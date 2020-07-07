//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "MagneticMoment.h"

registerMooseObject("HammerheadApp", MagneticMoment);

InputParameters
MagneticMoment::validParams()
{
  InputParameters params = VectorAuxKernel::validParams();

  params.addRequiredCoupledVar("magnetic_field", "The magnetic field (H).");

  return params;
}

MagneticMoment::MagneticMoment(const InputParameters & parameters)
  : VectorAuxKernel(parameters), _current_density(coupledCurl("magnetic_field"))
{
}

RealVectorValue
MagneticMoment::computeValue()
{
  // Access the gradient of the pressure at this quadrature point, then pull out the "component" of
  // it requested (x, y or z). Note, that getting a particular component of a gradient is done using
  // the parenthesis operator.
  return 0.5 * _q_point[_qp].cross(_current_density[_qp]);
}
