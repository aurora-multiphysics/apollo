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
  params.addRequiredParam<UserObjectName>("waveguide_properties",
                                          "The name of the user object for waveguide properties");  
  return params;
}

RW_TE10_Real::RW_TE10_Real(const InputParameters & parameters)
  : Function(parameters), 
   _wp(getUserObject<WaveguideProperties>("waveguide_properties"))
{
}

RealVectorValue
RW_TE10_Real::vectorValue(Real t, const Point & p) const
{
  Real gamma_im = _wp.getImagPropagationConstant();
  Real E0 = -sqrt(2*_wp._omega*_wp._mu0/(_wp._a*_wp._b*gamma_im));

  // Real E10_re = E0*sin(kc*p(1))*cos(gamma_im*p(0));
  // Real E10_im = E0*sin(kc*p(1))*sin(gamma_im*p(0));
  Real E10_re = E0*sin(M_PI*p(1)/_wp._a);

  return RealVectorValue(0, 0, E10_re);
  // return RealVectorValue(0, 0, 0);

  // return _function_ptr->evaluate<RealVectorValue>(t, p);
}

// Real
// RW_TE10_Real::value(Real /*t*/, const Point & p) const
// {
//   return _alpha * _alpha * libMesh::pi * libMesh::pi *
//          std::sin(_alpha * libMesh::pi * p(0)); // p(0) == x
// }
