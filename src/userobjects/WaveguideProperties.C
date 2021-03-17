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
  MooseEnum propagationdirection("x y z", "x");
  params.addRequiredParam<MooseEnum>("propagation_direction",
                             propagationdirection,
                             "Direction of propagation of wave at port.");  
  params.addRequiredParam<Real>("port_length", "The length in m of the port perpendicular to the propagation direction and applied E field.");
  params.addRequiredParam<Real>("port_width", "The length in m of the port along the axis of the applied E field.");
  params.addRequiredParam<Real>("frequency", "The input frequency in Hz.");
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
  MooseEnum propagation_direction = getParam<MooseEnum>("propagation_direction");

  if (propagation_direction == "x"){
    Real E10_re = E0*sin(kc*p(1))*cos(gamma_im*p(0));
    Real E10_im = -E0*sin(kc*p(1))*sin(gamma_im*p(0));
    return RealVectorValue(0, 0, E10_re);
  }
  else if (propagation_direction == "y"){
    Real E10_re = E0*sin(kc*p(2))*cos(gamma_im*p(1));
    Real E10_im = -E0*sin(kc*p(2))*sin(gamma_im*p(1));
    return RealVectorValue(E10_re, 0, 0);
  }
  else if (propagation_direction == "z"){
    Real E10_re = E0*sin(kc*p(0))*cos(gamma_im*p(2));
    Real E10_im = -E0*sin(kc*p(0))*sin(gamma_im*p(2));
    return RealVectorValue(0, E10_re, 0);
  }
  else{
    mooseError("Propagation direction not recognised. Please choose from (x, z)");
    return RealVectorValue(0, 0, 0);
  }
}

RealVectorValue
WaveguideProperties::getImagRWTE10(Real t, const Point & p) const
{
  Real gamma_im = getImagPropagationConstant();
  Real E0 = sqrt(2*_omega*_mu0/(_a*_b*gamma_im));
  Real kc = M_PI/_a;
  MooseEnum propagation_direction = getParam<MooseEnum>("propagation_direction");
  if (propagation_direction == "x"){
    Real E10_re = E0*sin(kc*p(1))*cos(gamma_im*p(0));
    Real E10_im = -E0*sin(kc*p(1))*sin(gamma_im*p(0));
    return RealVectorValue(0, 0, E10_im);
  }
  else if (propagation_direction == "y"){
    Real E10_re = E0*sin(kc*p(2))*cos(gamma_im*p(1));
    Real E10_im = -E0*sin(kc*p(2))*sin(gamma_im*p(1));
    return RealVectorValue(E10_im, 0, 0);
  }
  else if (propagation_direction == "z"){
    Real E10_re = E0*sin(kc*p(0))*cos(gamma_im*p(2));
    Real E10_im = -E0*sin(kc*p(0))*sin(gamma_im*p(2));
    return RealVectorValue(0, E10_im, 0);
  }
  else{
    mooseError("Propagation direction not recognised. Please choose from (x, z)");
    return RealVectorValue(0, 0, 0);
  }
}