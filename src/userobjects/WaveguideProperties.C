//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
//* Convert into Action; see INSAction for example.
#include "WaveguideProperties.h"

registerMooseObject("ApolloApp", WaveguideProperties);

const Real WaveguideProperties::_mu0 = 1.25663706e-6;
const Real WaveguideProperties::_epsilon0 = 8.85418782e-12;

InputParameters
WaveguideProperties::validParams()
{
  InputParameters params = GeneralUserObject::validParams();
  params.addRequiredParam<RealVectorValue>("port_length_vector", "The vector directed along the longest length of a rectangular waveguide port,"
                                "with magnitude equal to the length of this edge in m ");
  params.addRequiredParam<RealVectorValue>("port_width_vector", "The vector directed along the shortest length of a rectangular waveguide port,"
                                "with magnitude equal to the length of this edge in m ");
  params.addRequiredParam<Real>("frequency", "The input frequency in Hz.");
  return params;
}

WaveguideProperties::WaveguideProperties(const InputParameters & parameters)
  : GeneralUserObject(parameters),
  _a1(getParam<RealVectorValue>("port_length_vector")),
  _a2(getParam<RealVectorValue>("port_width_vector")),
  _a3(_a1.cross(_a2)),
  _a(sqrt(_a1*_a1)),
  _b(sqrt(_a2*_a2)),
  _omega(getParam<Real>("frequency")*(2*M_PI)),
  kc(M_PI/_a),
  k0(_omega*sqrt(_epsilon0*_mu0)),
  gamma_im(sqrt(k0*k0-kc*kc)),
  E0(sqrt(2*_omega*_mu0/(_a*_b*gamma_im))),
  V(_a1*_a2.cross(_a3)),
  _n(1),
  _m(0),
  k_a((M_PI*_n/V)*_a2.cross(_a3)),
  k_b((M_PI*_m/V)*_a3.cross(_a1)),
  k_c(gamma_im*_a3/sqrt(_a3*_a3)),
  E_hat(k_c.cross(k_a)/sqrt(k_c.cross(k_a)*k_c.cross(k_a)))
{
}

WaveguideProperties::~WaveguideProperties() {}

Real
WaveguideProperties::getImagPropagationConstant() const
{
  return gamma_im;
}

RealVectorValue
WaveguideProperties::getRealRWTE10(Real t, const Point & p) const
{
  RealVectorValue E10_re = E0*sin(k_a*p)*cos(k_c*p)*E_hat;
  return E10_re;
}

RealVectorValue
WaveguideProperties::getImagRWTE10(Real t, const Point & p) const
{
  RealVectorValue E10_im = -E0*sin(k_a*p)*sin(k_c*p)*E_hat;
  return E10_im;
}