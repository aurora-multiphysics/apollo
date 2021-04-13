//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

// Navier-Stokes includes
#include "CMAction.h"

#include "Maxwell.h"
#include "AddVariableAction.h"
#include "MooseObject.h"
#include "NonlinearSystemBase.h"

// MOOSE includes
#include "FEProblem.h"

#include "libmesh/fe.h"
#include "libmesh/vector_value.h"
#include "libmesh/string_to_enum.h"

registerMooseAction("ApolloApp", CMAction, "add_maxwell_variables");
registerMooseAction("ApolloApp", CMAction, "add_maxwell_ics");
registerMooseAction("ApolloApp", CMAction, "add_maxwell_kernels");
registerMooseAction("ApolloApp", CMAction, "add_maxwell_bcs");
registerMooseAction("ApolloApp", CMAction, "add_material");

InputParameters
CMAction::validParams()
{
  InputParameters params = Action::validParams();
  params.addClassDescription("This class allows us to have a section of the input file for "
                             "setting up incompressible Navier-Stokes equations.");

  MooseEnum families(AddVariableAction::getNonlinearVariableFamilies());
  MooseEnum orders(AddVariableAction::getNonlinearVariableOrders());
  params.addParam<MooseEnum>(
      "family", families, "Specifies the family of FE shape functions to use for this variable");
  params.addParam<MooseEnum>("order",
                             orders,
                             "Specifies the order of the FE shape function to use "
                             "for this variable (additional orders not listed are "
                             "allowed)");
  params.addParam<std::vector<BoundaryName>>(
      "pec_boundaries", std::vector<BoundaryName>(), "Boundaries at which perfect electrical conductor "
      "(PEC) boundary conditions are applied");
  params.addParam<std::vector<BoundaryName>>(
      "wg_input_boundaries", std::vector<BoundaryName>(), "Boundaries at acting as waveguide input ports.");
  params.addParam<std::vector<BoundaryName>>(
      "wg_output_boundaries", std::vector<BoundaryName>(), "Boundaries at acting as waveguide output ports.");
  params.addParam<Real>(
      "pec_penalty", 0, "Penalty coefficient for PEC boundary conditions");
  params.addParamNamesToGroup("pec_boundaries pec_penalty",
                        "BoundaryCondition");
  params.addParamNamesToGroup(
      "family order", "Variable");
  return params;
}

CMAction::CMAction(InputParameters parameters)
  : Action(parameters),
  _fe_type(AddVariableAction::feType(parameters)),
    _pec_boundaries(getParam<std::vector<BoundaryName>>("pec_boundaries")),
    _pec_penalty(getParam<Real>("pec_penalty")),
    _wg_input_ports(getParam<std::vector<BoundaryName>>("wg_input_boundaries")),
    _wg_output_ports(getParam<std::vector<BoundaryName>>("wg_output_boundaries"))
{
}

void
CMAction::act()
{
  if (_current_task == "add_maxwell_variables")
  {
    auto var_type = AddVariableAction::determineType(_fe_type, 1);
    auto base_params = _factory.getValidParams(var_type);
    base_params.set<MooseEnum>("family") = Moose::stringify(_fe_type.family);
    base_params.set<MooseEnum>("order") = _fe_type.order.get_order();

    // add primal variables
    InputParameters params(base_params);
    params.set<MooseEnum>("order") = _fe_type.order.get_order();

    int order = _fe_type.order.get_order();
    params.set<MooseEnum>("order") = order;
    _problem->addVariable(var_type, Maxwell::e_field_re, params);
    _problem->addVariable(var_type, Maxwell::e_field_im, params);

    const std::string class_name = "WaveguideProperties";
    InputParameters wg_params = _app.getFactory().getValidParams(class_name);
    wg_params.set<RealVectorValue>("port_length_vector") = RealVectorValue(22.86e-3, 0, 0);
    wg_params.set<RealVectorValue>("port_width_vector") = RealVectorValue(0, 10.16e-3, 0);
    wg_params.set<Real>("frequency") = 9.3e9;
    _problem->addUserObject(class_name, "Waveguide", wg_params);

    // // for non-stablized form, the FE order for pressure need to be at least one order lower
    // int order = _fe_type.order.get_order();
    // if (!getParam<bool>("pspg"))
    //   order -= 1;
    // params.set<MooseEnum>("order") = order;
    // params.set<std::vector<Real>>("scaling") = {getParam<Real>("pressure_scaling")};
    // _problem->addVariable(var_type, NS::pressure, params);
  }

  if (_current_task == "add_maxwell_kernels")
  {
    addCMKernels();
  }

  if (_current_task == "add_maxwell_bcs")
  {
    if (_pec_boundaries.size() > 0)
      addCMPECBC();
    if (_wg_input_ports.size() > 0 || _wg_output_ports.size() > 0)
      addCMWaveguidePortsBC();
  }
}

void
CMAction::addCMPECBC()
{
  std::string bc_type;
  for (unsigned int i = 0; i < _pec_boundaries.size(); ++i)
  {
    bc_type = "VectorTangentialPenaltyDirichletRealBC";
    InputParameters re_params = _factory.getValidParams(bc_type);
    re_params.set<NonlinearVariableName>("variable") = Maxwell::e_field_re;
    re_params.set<std::vector<VariableName>>("v") = {Maxwell::e_field_im};
    re_params.set<Real>("penalty") = _pec_penalty;
    re_params.set<std::vector<BoundaryName>>("boundary") = {_pec_boundaries[i]};
    _problem->addBoundaryCondition(bc_type, "PEC_real_" + _pec_boundaries[i], re_params);

    bc_type = "VectorTangentialPenaltyDirichletImagBC";
    InputParameters im_params = _factory.getValidParams(bc_type);
    im_params.set<NonlinearVariableName>("variable") = Maxwell::e_field_im;
    im_params.set<std::vector<VariableName>>("v") = {Maxwell::e_field_re};
    im_params.set<Real>("penalty") = _pec_penalty;
    im_params.set<std::vector<BoundaryName>>("boundary") = {_pec_boundaries[i]};
    _problem->addBoundaryCondition(bc_type, "PEC_imag_" + _pec_boundaries[i], im_params);
  }
}

void
CMAction::addCMWaveguidePortsBC()
{
  std::string bc_type;
  for (unsigned int i = 0; i < _wg_input_ports.size(); ++i)
  {
    bc_type = "RobinTE10RealBC";
    InputParameters re_params = _factory.getValidParams(bc_type);
    re_params.set<NonlinearVariableName>("variable") = Maxwell::e_field_re;
    re_params.set<std::vector<VariableName>>("v") = {Maxwell::e_field_im};
    re_params.set<bool>("input_port") = true;
    re_params.set<UserObjectName>("waveguide_properties") = "Waveguide";
    re_params.set<std::vector<BoundaryName>>("boundary") = {_wg_input_ports[i]};
    _problem->addBoundaryCondition(bc_type, "RWTE10_in_real_" + _wg_input_ports[i], re_params);

    bc_type = "RobinTE10ImagBC";
    InputParameters im_params = _factory.getValidParams(bc_type);
    im_params.set<NonlinearVariableName>("variable") = Maxwell::e_field_im;
    im_params.set<std::vector<VariableName>>("v") = {Maxwell::e_field_re};
    im_params.set<bool>("input_port") = true;
    im_params.set<UserObjectName>("waveguide_properties") = "Waveguide";
    im_params.set<std::vector<BoundaryName>>("boundary") = {_wg_input_ports[i]};
    _problem->addBoundaryCondition(bc_type, "RWTE10_in_imag_" + _wg_input_ports[i], im_params);
  }

  for (unsigned int i = 0; i < _wg_output_ports.size(); ++i)
  {
    bc_type = "RobinTE10RealBC";
    InputParameters re_params = _factory.getValidParams(bc_type);
    re_params.set<NonlinearVariableName>("variable") = Maxwell::e_field_re;
    re_params.set<std::vector<VariableName>>("v") = {Maxwell::e_field_im};
    re_params.set<bool>("input_port") = false;
    re_params.set<UserObjectName>("waveguide_properties") = "Waveguide";
    re_params.set<std::vector<BoundaryName>>("boundary") = {_wg_output_ports[i]};
    _problem->addBoundaryCondition(bc_type, "RWTE10_out_real_" + _wg_output_ports[i], re_params);

    bc_type = "RobinTE10ImagBC";
    InputParameters im_params = _factory.getValidParams(bc_type);
    im_params.set<NonlinearVariableName>("variable") = Maxwell::e_field_im;
    im_params.set<std::vector<VariableName>>("v") = {Maxwell::e_field_re};
    im_params.set<bool>("input_port") = false;
    im_params.set<UserObjectName>("waveguide_properties") = "Waveguide";
    im_params.set<std::vector<BoundaryName>>("boundary") = {_wg_output_ports[i]};
    _problem->addBoundaryCondition(bc_type, "RWTE10_out_imag_" + _wg_output_ports[i], im_params);
  }
}

void
CMAction::addCMKernels()
{
  std::string kernel_type = "ComplexMaxwellReal";
  InputParameters re_params = _factory.getValidParams(kernel_type);
  re_params.set<NonlinearVariableName>("variable") = Maxwell::e_field_re;
  re_params.set<std::vector<VariableName>>("v") = {Maxwell::e_field_im};
  re_params.set<UserObjectName>("waveguide_properties") = "Waveguide";
  _problem->addKernel(kernel_type, "Real", re_params);

  kernel_type = "ComplexMaxwellImag";
  InputParameters im_params = _factory.getValidParams(kernel_type);
  im_params.set<NonlinearVariableName>("variable") = Maxwell::e_field_im;
  im_params.set<std::vector<VariableName>>("v") = {Maxwell::e_field_re};
  im_params.set<UserObjectName>("waveguide_properties") = "Waveguide";
  _problem->addKernel(kernel_type, "Imag", im_params);
}
