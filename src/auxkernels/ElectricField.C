//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ElectricField.h"

registerMooseObject("ApolloApp", ElectricField);

InputParameters
ElectricField::validParams()
{
  InputParameters params = VectorAuxKernel::validParams();
  params.addRequiredCoupledVar("magnetic_vector_potential", "The magnetic vector potential (A).");
  params.addCoupledVar("electric_scalar_potential",0,
                               "The electric scalar potential (V).");
  return params;
}

ElectricField::ElectricField(const InputParameters & parameters)
  : VectorAuxKernel(parameters),
    _a_dot(coupledVectorDot("magnetic_vector_potential")),
    _grad_v(coupledGradient("electric_scalar_potential"))
{
}

RealVectorValue
ElectricField::computeValue()
{
  // Access the gradient of the pressure at this quadrature point, then pull out the "component" of
  // it requested (x, y or z). Note, that getting a particular component of a gradient is done using
  // the parenthesis operator.
  return -_a_dot[_qp] - _grad_v[_qp];
}
