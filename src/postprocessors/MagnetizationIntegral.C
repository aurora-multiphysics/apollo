//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
//* Replace with ElementIntegralArrayVariablePostprocessor?

#include "MagnetizationIntegral.h"

registerMooseObject("HammerheadApp", MagnetizationIntegral);

defineLegacyParams(MagnetizationIntegral);

InputParameters
MagnetizationIntegral::validParams()
{
  InputParameters params = ElementIntegralPostprocessor::validParams();
  MooseEnum component("x y z");
  params.addRequiredCoupledVar("magnetic_field", "The magnetic field (H) in the system.");
  params.addRequiredParam<MooseEnum>(
      "component", component, "The desired component of magnetization.");
  params.addClassDescription("Computes the magnetisation (m) of the block.");
  return params;
}

MagnetizationIntegral::MagnetizationIntegral(const InputParameters & parameters)
  : ElementIntegralPostprocessor(parameters),
    _component(getParam<MooseEnum>("component")),
    _u(coupledVectorValue("magnetic_field")),
    _curl_u(coupledCurl("magnetic_field"))
{
}

Real
MagnetizationIntegral::computeQpIntegral()
{
  return 0.5 * _q_point[_qp].cross(_curl_u[_qp])(_component);
}
