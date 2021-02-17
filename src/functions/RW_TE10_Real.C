//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
//* RW_TE10_Real for fundamental mode of rectangular waveguide
#include "RW_TE10_Real.h"

registerMooseObject("HammerheadApp", RW_TE10_Real);

template <>
InputParameters
validParams<RW_TE10_Real>()
{
  InputParameters params = validParams<Function>();
  params.addParam<Real>("port_length", 1.0, "The length of the port in the x direction in m.");
  params.addParam<Real>("port_width", 1.0, "The length of the port in the y direction in m.");
  params.addParam<Real>("frequency", 1.0, "The input frequency in Hz.");
  return params;
}

RW_TE10_Real::RW_TE10_Real(const InputParameters & parameters)
  : Function(parameters), 
  _a(getParam<Real>("port_length")),
  _b(getParam<Real>("port_width")),
  _omega(getParam<Real>("frequency")/(2*M_PI))
{
}

RealVectorValue
RW_TE10_Real::vectorValue(Real t, const Point & p) const
{
  Real mu0 = 1.25663706e-6;
  Real epsilon0 = 8.85418782e-12;
  Real kc = M_PI/_a;
  Real k0 = _omega*sqrt(epsilon0*mu0);
  Real gamma_im = sqrt(k0*k0-kc*kc);
  Real E0 = -sqrt(2*_omega*mu0/(_a*_b*gamma_im));
  Real E10_re = E0*sin(kc*p(0))*cos(gamma_im*p(2));
  Real E10_im = E0*sin(kc*p(0))*sin(gamma_im*p(2));

  return RealVectorValue(0, E10_re, 0);
  // return _function_ptr->evaluate<RealVectorValue>(t, p);
}

// Real
// RW_TE10_Real::value(Real /*t*/, const Point & p) const
// {
//   return _alpha * _alpha * libMesh::pi * libMesh::pi *
//          std::sin(_alpha * libMesh::pi * p(0)); // p(0) == x
// }
