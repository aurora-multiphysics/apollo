//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
//* PropagationConstant_Imag for fundamental mode of rectangular waveguide
#include "PropagationConstant_Imag.h"

registerMooseObject("HammerheadApp", PropagationConstant_Imag);

template <>
InputParameters
validParams<PropagationConstant_Imag>()
{
  InputParameters params = validParams<Function>();
  params.addParam<Real>("port_length", 1.0, "The length of the port in the x direction in m.");
  params.addParam<Real>("port_width", 1.0, "The length of the port in the y direction in m.");
  params.addParam<Real>("frequency", 1.0, "The input frequency in Hz.");

  return params;
}

PropagationConstant_Imag::PropagationConstant_Imag(const InputParameters & parameters)
  : Function(parameters), 
  _a(getParam<Real>("port_length")),
  _b(getParam<Real>("port_width")),
  _omega(getParam<Real>("frequency")/(2*M_PI))
{
}

Real
PropagationConstant_Imag::value(Real t, const Point & p) const
{
  Real mu0 = 1.25663706e-6;
  Real epsilon0 = 8.85418782e-12;
  Real kc = M_PI/_a;
  Real k0 = _omega*sqrt(epsilon0*mu0);
  Real gamma_im = sqrt(k0*k0-kc*kc);
  return sqrt(k0*k0-kc*kc); // p(0) == x
}
