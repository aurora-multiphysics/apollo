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

#include "HarmonicMaxwellBase.h"
#include "Function.h"
#include "Assembly.h"

registerMooseObject("ApolloApp", HarmonicMaxwellBase);

InputParameters
HarmonicMaxwellBase::validParams()
{
  InputParameters params = VectorKernel::validParams();
  params.addClassDescription("This class computes various components of the"
                             "Maxwell equations which can then be assembled "
                             "together in child classes.");
  params.addRequiredCoupledVar("v", "Coupled vector variable");
  return params;
}

HarmonicMaxwellBase::HarmonicMaxwellBase(const InputParameters & parameters)
  : VectorKernel(parameters),
    _curl_phi(_assembly.curlPhi(_var)),
    _curl_test(_var.curlPhi()),
    _curl_u(_is_implicit ? _var.curlSln() : _var.curlSlnOld()),
    _v_id(coupled("v")),
    _v(coupledVectorValue("v")),
    _curl_v(coupledCurl("v"))
{
}

Real
HarmonicMaxwellBase::curlCurlTerm()
{
  return _curl_u[_qp] * _curl_test[_i][_qp];
}

Real
HarmonicMaxwellBase::dCurlCurlDU()
{
  return _curl_phi[_j][_qp] * _curl_test[_i][_qp];
}

Real
HarmonicMaxwellBase::gaugePenaltyTerm()
{
  return _grad_u[_qp].tr() * _grad_test[_i][_qp].tr();
}

Real
HarmonicMaxwellBase::dGaugePenaltyDU()
{
  return _grad_phi[_j][_qp].tr() * _grad_test[_i][_qp].tr();
}

Real
HarmonicMaxwellBase::coupledCurlCurlTerm()
{
  return _curl_v[_qp] * _curl_test[_i][_qp];
}

Real
HarmonicMaxwellBase::dCoupledCurlCurlDU()
{
  return _curl_phi[_j][_qp] * _curl_test[_i][_qp];
}

Real
HarmonicMaxwellBase::computeQpResidual() {return 0;}

Real
HarmonicMaxwellBase::computeQpJacobian(){return 0;}