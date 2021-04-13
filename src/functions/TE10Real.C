//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "TE10Real.h"

registerMooseObject("ApolloApp", TE10Real);

InputParameters
TE10Real::validParams()
{
  InputParameters params = Function::validParams();
  params.addClassDescription(
      "A function that returns the real part of the TE10 mode in a waveguide.");
  MooseEnum propagationdirection("x y z", "x");
  params.addRequiredParam<MooseEnum>("propagation_direction",
                             propagationdirection,
                             "Direction of propagation of wave at port.");
  params.addRequiredParam<Real>("magnitude", "The magnitude of the applied E field.");
  params.addRequiredParam<Real>("imag_propagation_constant", "The length in m of the port along the axis of the applied E field.");
  params.addRequiredParam<Real>("cutoff_wavenumber", "The input frequency in Hz.");
  return params;
}

TE10Real::TE10Real(const InputParameters & parameters)
  : Function(parameters),
  kc(getParam<Real>("cutoff_wavenumber")),
  gamma_im(getParam<Real>("imag_propagation_constant")),
  E0(getParam<Real>("magnitude")),
  propagation_direction(getParam<MooseEnum>("propagation_direction"))
{
}

RealVectorValue
TE10Real::vectorValue(Real t , const Point & p) const
{
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
