//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

// Navier-Stokes includes
#include "AVAction.h"

#include "Maxwell.h"
#include "AddVariableAction.h"
#include "MooseObject.h"
#include "NonlinearSystemBase.h"

// MOOSE includes
#include "FEProblem.h"

#include "libmesh/fe.h"
#include "libmesh/vector_value.h"
#include "libmesh/string_to_enum.h"

registerMooseAction("ApolloApp", AVAction, "add_maxwell_variables");
registerMooseAction("ApolloApp", AVAction, "add_maxwell_ics");
registerMooseAction("ApolloApp", AVAction, "add_maxwell_kernels");
registerMooseAction("ApolloApp", AVAction, "add_maxwell_bcs");
registerMooseAction("ApolloApp", AVAction, "add_material");

InputParameters
AVAction::validParams()
{
  InputParameters params = Action::validParams();
  params.addClassDescription("This class allows us to have a section of the input file for "
                             "setting up incompressible Navier-Stokes equations.");

  MooseEnum families(AddVariableAction::getNonlinearVariableFamilies());
  MooseEnum orders(AddVariableAction::getNonlinearVariableOrders());
  params.addParam<MooseEnum>(
      "scalar_family", families, "Specifies the family of FE shape functions to use for vector variables");
  params.addParam<MooseEnum>("scalar_order",
                             orders,
                             "Specifies the order of the FE shape function to use "
                             "for vector variables (additional orders not listed are "
                             "allowed)");
  params.addParam<MooseEnum>(
      "vector_family", families, "Specifies the family of FE shape functions to use for vector variables");
  params.addParam<MooseEnum>("vector_order",
                             orders,
                             "Specifies the order of the FE shape function to use "
                             "for vector variables (additional orders not listed are "
                             "allowed)");
  params.addParam<std::vector<BoundaryName>>(
      "tangent_h_boundaries", std::vector<BoundaryName>(), "Boundaries through which the tangential H field is constrained passes"
      "(H×n=H_ext×n)");
  params.addParam<Real>(
      "tangent_h_penalty", 0, "Penalty coefficient for tangential H field boundary conditions");

  params.addParam<std::vector<BoundaryName>>(
      "zero_flux_boundaries", std::vector<BoundaryName>(), "Boundaries through which zero magnetic flux passes"
      "(B·n = 0)");
  params.addParam<Real>(
      "zero_flux_penalty", 0, "Penalty coefficient for zero flux boundary conditions");
  params.addParam<std::vector<BoundaryName>>(
      "electric_potential_boundaries", std::vector<BoundaryName>(), "Boundaries on which the electric potential is defined."
      "(V=V_ext)");
  params.addParamNamesToGroup("tangent_h_boundaries tangent_h_penalty zero_flux_boundaries zero_flux_penalty electric_potential_boundaries",
                        "BoundaryCondition");
  params.addParamNamesToGroup(
      "scalar_family scalar_order vector_family vector_order", "Variable");
  return params;
}

AVAction::AVAction(InputParameters parameters)
  : Action(parameters),
    _vector_fe_type(Utility::string_to_enum<Order>(getParam<MooseEnum>("vector_order")),
                    Utility::string_to_enum<FEFamily>(getParam<MooseEnum>("vector_family"))),
    _scalar_fe_type(Utility::string_to_enum<Order>(getParam<MooseEnum>("scalar_order")),
                    Utility::string_to_enum<FEFamily>(getParam<MooseEnum>("scalar_family"))),
  _tangent_h_boundaries(getParam<std::vector<BoundaryName>>("tangent_h_boundaries")),
  _tangent_h_penalty(getParam<Real>("tangent_h_penalty")),
  _zero_flux_boundaries(getParam<std::vector<BoundaryName>>("zero_flux_boundaries")),
  _zero_flux_penalty(getParam<Real>("zero_flux_penalty")),
  _electric_potential_boundaries(getParam<std::vector<BoundaryName>>("electric_potential_boundaries"))
{
}

void
AVAction::act()
{
  if (_current_task == "add_maxwell_variables")
  {
    auto _vector_var_type = AddVariableAction::determineType(_vector_fe_type, 1);
    auto _vector_base_params = _factory.getValidParams(_vector_var_type);
    _vector_base_params.set<MooseEnum>("family") = Moose::stringify(_vector_fe_type.family);
    _vector_base_params.set<MooseEnum>("order") = _vector_fe_type.order.get_order();
    InputParameters _vector_params(_vector_base_params);
    _vector_params.set<MooseEnum>("order") = _vector_fe_type.order.get_order();
    _problem->addVariable(_vector_var_type, Maxwell::magnetic_vector_potential, _vector_params);

    auto _scalar_var_type = AddVariableAction::determineType(_scalar_fe_type, 1);
    auto _scalar_base_params = _factory.getValidParams(_scalar_var_type);
    _scalar_base_params.set<MooseEnum>("family") = Moose::stringify(_scalar_fe_type.family);
    _scalar_base_params.set<MooseEnum>("order") = _scalar_fe_type.order.get_order();
    InputParameters _scalar_params(_scalar_base_params);
    _scalar_params.set<MooseEnum>("order") = _scalar_fe_type.order.get_order();
    _problem->addVariable(_scalar_var_type, Maxwell::electric_scalar_potential, _scalar_params);
  }

  if (_current_task == "add_maxwell_kernels")
  {
    addAVKernels();
  }

  if (_current_task == "add_maxwell_bcs")
  {
    if (_tangent_h_boundaries.size() > 0)
      addTangentialHBC();
    if (_zero_flux_boundaries.size() > 0)
      addZeroFluxBC();
    if (_electric_potential_boundaries.size() > 0)
      addElectricPotentialBC();
  }
}

void
AVAction::addTangentialHBC()
{
  // Set H×n at boundary: ν∇×A×n (Neumann) with gauge constraint A·n=0 (Dirichlet)
  std::string tngt_bc_type = "VectorTangentialPenaltyDirichletBC";
  std::string norm_bc_type = "VectorFunctionDirichletBC";
  for (unsigned int i = 0; i < _tangent_h_boundaries.size(); ++i)
  {
    InputParameters tngt_params = _factory.getValidParams(tngt_bc_type);
    tngt_params.set<NonlinearVariableName>("variable") = Maxwell::magnetic_vector_potential;
    tngt_params.set<Real>("penalty") = _tangent_h_penalty;
    tngt_params.set<std::vector<BoundaryName>>("boundary") = { _tangent_h_boundaries[i]};
    _problem->addBoundaryCondition(tngt_bc_type, "tangent_H_field" + _tangent_h_boundaries[i], tngt_params);

    InputParameters norm_params = _factory.getValidParams(norm_bc_type);
    norm_params.set<NonlinearVariableName>("variable") = Maxwell::magnetic_vector_potential;
    norm_params.set<std::vector<BoundaryName>>("boundary") = { _tangent_h_boundaries[i]};
    _problem->addBoundaryCondition(norm_bc_type, "tangent_H_gauging" + _tangent_h_boundaries[i], norm_params);
  }
}

void
AVAction::addZeroFluxBC()
{
  // Set B·n=0 at boundary: A×n=0 (Dirichlet) with gauge imposed by ν∇·A=0 (Neumann)
  // std::string tngt_bc_type = "VectorTangentialPenaltyDirichletBC";
  // std::string norm_bc_type = "VectorFunctionDirichletBC";
  // for (unsigned int i = 0; i < _tangent_h_boundaries.size(); ++i)
  // {
  //   InputParameters tngt_params = _factory.getValidParams(tngt_bc_type);
  //   tngt_params.set<NonlinearVariableName>("variable") = Maxwell::magnetic_vector_potential;
  //   tngt_params.set<Real>("penalty") = _tangent_h_penalty;
  //   tngt_params.set<std::vector<BoundaryName>>("boundary") = { _tangent_h_boundaries[i]};
  //   _problem->addBoundaryCondition(tngt_bc_type, "tangent_H_field" + _tangent_h_boundaries[i], tngt_params);

  //   InputParameters norm_params = _factory.getValidParams(norm_bc_type);
  //   norm_params.set<NonlinearVariableName>("variable") = Maxwell::magnetic_vector_potential;
  //   norm_params.set<std::vector<BoundaryName>>("boundary") = { _tangent_h_boundaries[i]};
  //   _problem->addBoundaryCondition(norm_bc_type, "tangent_H_gauging" + _tangent_h_boundaries[i], norm_params);
  // }
}

void
AVAction::addElectricPotentialBC()
{
  //Dirichlet BC for the scalar potential that fixes the electric potential
  //on a surface.
}

// void
// AVAction::addElectricCurrentBC()
// {
//   //Neumann BC for the scalar potential that fixes the integrated
//   //current density over a surface.
// }

void
AVAction::addAVKernels()
{
  std::string vector_kernel_type = "EddyAVFaraday";
  InputParameters vector_params = _factory.getValidParams(vector_kernel_type);
  vector_params.set<NonlinearVariableName>("variable") = Maxwell::magnetic_vector_potential;
  vector_params.set<std::vector<VariableName>>("scalar_potential") = {Maxwell::electric_scalar_potential};
  vector_params.set<bool>("gauge_penalty") = true;
  vector_params.set<bool>("quasistationary") = true;
  _problem->addKernel(vector_kernel_type, "EddyAVFaraday", vector_params);

  std::string scalar_kernel_type = "EddyAVGauss";
  InputParameters scalar_params = _factory.getValidParams(scalar_kernel_type);
  scalar_params.set<NonlinearVariableName>("variable") = Maxwell::electric_scalar_potential;
  scalar_params.set<std::vector<VariableName>>("vector_potential") = {Maxwell::magnetic_vector_potential};
  _problem->addKernel(scalar_kernel_type, "EddyAVGauss", scalar_params);


  // kernel_type = "ComplexMaxwellImag";
  // InputParameters im_params = _factory.getValidParams(kernel_type);
  // im_params.set<NonlinearVariableName>("variable") = Maxwell::e_field_im;
  // im_params.set<std::vector<VariableName>>("v") = {Maxwell::e_field_re};
  // im_params.set<UserObjectName>("waveguide_properties") = getParam<UserObjectName>("wg_properties");
  // _problem->addKernel(kernel_type, "Imag", im_params);
}
