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
  params.addParamNamesToGroup(
      "family order", "Variable");
  return params;
}

CMAction::CMAction(InputParameters parameters)
  : Action(parameters),
  _fe_type(AddVariableAction::feType(parameters))
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



    // // for non-stablized form, the FE order for pressure need to be at least one order lower
    // int order = _fe_type.order.get_order();
    // if (!getParam<bool>("pspg"))
    //   order -= 1;
    // params.set<MooseEnum>("order") = order;
    // params.set<std::vector<Real>>("scaling") = {getParam<Real>("pressure_scaling")};
    // _problem->addVariable(var_type, NS::pressure, params);
  }

  // if (_current_task == "add_maxwell_kernels")
  // {
  //   if (_type == "transient")
  //     addCMTimeKernels();

  //   // Add all the inviscid flux Kernels.
  //   addCMMass();
  //   addCMMomentum();

  //   if (getParam<bool>("add_temperature_equation"))
  //     addCMTemperature();

  //   if (_use_ad && getParam<bool>("add_standard_velocity_variables_for_ad"))
  //     addCMVelocityAux();
  // }

  // if (_current_task == "add_maxwell_bcs")
  // {
  //   if (_velocity_boundary.size() > 0)
  //     addCMVelocityBC();

  //   if (_has_pinned_node)
  //     addCMPinnedPressureBC();

  //   if (_no_bc_boundary.size() > 0)
  //     addCMNoBCBC();

  //   if (_pressure_boundary.size() > 0)
  //     addCMPressureBC();

  //   if (getParam<bool>("add_temperature_equation"))
  //   {
  //     if (_fixed_temperature_boundary.size() > 0)
  //       addCMTemperatureBC();
  //   }
  // }
  //   if (getParam<bool>("supg") || getParam<bool>("pspg"))
  //   {
  //     InputParameters params = _factory.getValidParams("CMADTauMaterial");
  //     set_common_parameters(params);
  //     params.set<Real>("alpha") = getParam<Real>("alpha");
  //     _problem->addMaterial("CMADTauMaterial", "ins_ad_material", params);
  //   }
  //   else
  //   {
  //     InputParameters params = _factory.getValidParams("CMADMaterial");
  //     set_common_parameters(params);
  //     _problem->addMaterial("CMADMaterial", "ins_ad_material", params);
  //   }
  // }
}

