//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "WaveguideProperties.h"

registerMooseObject("HammerheadApp", WaveguideProperties);

const Real WaveguideProperties::_mu0 = 1.25663706e-6;
const Real WaveguideProperties::_epsilon0 = 8.85418782e-12;

InputParameters
WaveguideProperties::validParams()
{
  InputParameters params = GeneralUserObject::validParams();
  params.addParam<Real>("port_length", 1.0, "The length of the port in the x direction in m.");
  params.addParam<Real>("port_width", 1.0, "The length of the port in the y direction in m.");
  params.addParam<Real>("frequency", 1.0, "The input frequency in Hz.");

  return params;
}

WaveguideProperties::WaveguideProperties(const InputParameters & parameters)
  : GeneralUserObject(parameters),
  _a(getParam<Real>("port_length")),
  _b(getParam<Real>("port_width")),
  _omega(getParam<Real>("frequency")*(2*M_PI))
{
}

WaveguideProperties::~WaveguideProperties() {}

Real
WaveguideProperties::getImagPropagationConstant() const
{
  Real kc = M_PI/_a;
  Real k0 = _omega*sqrt(_epsilon0*_mu0); //2 pi f/c 
  return sqrt(k0*k0-kc*kc);
}

RealVectorValue
WaveguideProperties::getRealRWTE10(Real t, const Point & p) const
{
  Real gamma_im = getImagPropagationConstant();
  Real E0 = sqrt(2*_omega*_mu0/(_a*_b*gamma_im));
  Real kc = M_PI/_a;
  Real E10_re = E0*sin(kc*p(1))*cos(gamma_im*p(0));
  Real E10_im = -E0*sin(kc*p(1))*sin(gamma_im*p(0));
  // Real E10_re = E0*sin(M_PI*p(1)/_a);
  return RealVectorValue(0, 0, E10_re);

  // return RealVectorValue(0, 0, (E10_re+E10_im)/sqrt(2));
}

RealVectorValue
WaveguideProperties::getImagRWTE10(Real t, const Point & p) const
{
  Real gamma_im = getImagPropagationConstant();
  Real E0 = sqrt(2*_omega*_mu0/(_a*_b*gamma_im));
  Real kc = M_PI/_a;

  Real E10_re = E0*sin(kc*p(1))*cos(gamma_im*p(0));
  Real E10_im = -E0*sin(kc*p(1))*sin(gamma_im*p(0));
  // Real E10_re = E0*sin(M_PI*p(1)/_a);

  return RealVectorValue(0, 0, E10_im);
  // return RealVectorValue(0, 0, (E10_re+E10_im)/sqrt(2));  
}