//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

//* Solves:
//* curl(a * curl u) - grad(a * div u) + d/dt s*( u + grad p) = 0
//* div (s * (u + grad p) )= 0
//*
//* in weak form:
//* (a * curl u, curl v) + (a * div u, div v) + (d/dt s*( u + grad p), v)
//* - <(a*curl u) x n, v> - <a*div u, v.n>  = 0
//* (d/dt s*( u + grad p), grad w) - <d/dt s*( u + grad p) . n, w> =0

//* For T-phi formulation, u = T, p = omega, a = rho, s = mu
//* and H = T0 + T - grad phi = u + grad p

//* For A-V formulation, u = A, p = int(V dt), a = mu^-1, s = sigma
//* B = curl A

#include "HarmonicMaxwellReal.h"
#include "Function.h"
#include "Assembly.h"

registerMooseObject("HammerheadApp", HarmonicMaxwellReal);

InputParameters
HarmonicMaxwellReal::validParams()
{
  InputParameters params = HarmonicMaxwellBase::validParams();
  params.addClassDescription("This class computes various components of the"
                             "Maxwell equations which can then be assembled "
                             "together in child classes.");
  params.addParam<Real>("frequency", 0, "The frequency of the EM mode in Hz.");
  return params;
}

HarmonicMaxwellReal::HarmonicMaxwellReal(const InputParameters & parameters)
  : HarmonicMaxwellBase(parameters),
  _omega(getParam<Real>("frequency")/(2*M_PI)),
  _sigma_re(getMaterialProperty<Real>("real_conductivity")),
  _sigma_im(getMaterialProperty<Real>("imag_conductivity")),
  _epsilon_re(getMaterialProperty<Real>("real_permittivity")),
  _epsilon_im(getMaterialProperty<Real>("imag_permittivity")),
  _nu_re(getMaterialProperty<Real>("real_reluctance")),
  _nu_im(getMaterialProperty<Real>("imag_reluctance"))     
{
}

Real
HarmonicMaxwellReal::computeQpResidual()
{
  return _nu_re[_qp]*HarmonicMaxwellBase::curlCurlTerm() 
       - _nu_im[_qp]*HarmonicMaxwellBase::coupledCurlCurlTerm()
       -_omega*_omega*(_epsilon_re[_qp]*_u[_qp]-_epsilon_im[_qp]*_v[_qp])*_test[_i][_qp];
}

Real
HarmonicMaxwellReal::computeQpJacobian()
{
  return _nu_re[_qp]*HarmonicMaxwellBase::dCurlCurlDU() 
       -_omega*_omega*_epsilon_re[_qp]*_phi[_j][_qp]*_test[_i][_qp];
}

Real
HarmonicMaxwellReal::computeQpOffDiagJacobian(unsigned int jvar)
{
  if (jvar == _v_id)
  return - _nu_im[_qp]*HarmonicMaxwellBase::dCoupledCurlCurlDU()
       +_omega*_omega*_epsilon_im[_qp]*_phi[_j][_qp]*_test[_i][_qp];
  return 0.0;
}