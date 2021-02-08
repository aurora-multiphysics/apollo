//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "MagneticField.h"

registerMooseObject("HammerheadApp", MagneticField);

InputParameters
MagneticField::validParams()
{
  InputParameters params = VectorAuxKernel::validParams();
  params.addRequiredCoupledVar("electric_vector_potential", "The electric vector potential (T).");
  params.addCoupledVar("magnetic_scalar_potential",0,
                               "The magnetic scalar potential (omega).");
  return params;
}

MagneticField::MagneticField(const InputParameters & parameters)
  : VectorAuxKernel(parameters),
    _T(coupledVectorValue("electric_vector_potential")),
    _grad_omega(coupledGradient("magnetic_scalar_potential"))
{
}

RealVectorValue
MagneticField::computeValue()
{
  // Access the gradient of the pressure at this quadrature point, then pull out the "component" of
  // it requested (x, y or z). Note, that getting a particular component of a gradient is done using
  // the parenthesis operator.
  return _T[_qp] + _grad_omega[_qp];
}
