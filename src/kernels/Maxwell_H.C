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

#include "Maxwell_H.h"
#include "Function.h"
#include "Assembly.h"

registerMooseObject("HammerheadApp", Maxwell_H);

InputParameters
Maxwell_H::validParams()
{
  InputParameters params = CurlCurl::validParams();
  return params;
}

Maxwell_H::Maxwell_H(const InputParameters & parameters)
  : VectorTimeKernel(parameters),
    _curl_curl(parameters),
    _gauge_penalty(parameters),
    _curl_phi(_assembly.curlPhi(_var)),
    _curl_test(_var.curlPhi()),
    _curl_u(_is_implicit ? _var.curlSln() : _var.curlSlnOld()),
    _u_dot(_var.uDot()),
    _du_dot_du(_var.duDotDu()),
    _rho(getMaterialProperty<Real>("resistivity")),
    _mu(getMaterialProperty<Real>("permeability"))
{
  // use component-wise curl on phi, u and test?
}

Real
Maxwell_H::computeQpResidual()
{
  return _rho[_qp] * (_curl_u[_qp] * _curl_test[_i][_qp] + _grad_u[_qp].tr() * _grad_test[_i][_qp].tr())
         + _mu[_qp] * _test[_i][_qp] * _u_dot[_qp]  ;
}

Real
Maxwell_H::computeQpJacobian()
{
  return _rho[_qp] * (_curl_phi[_j][_qp] * _curl_test[_i][_qp] +
                      _grad_phi[_j][_qp].tr() * _grad_test[_i][_qp].tr()) +
         _mu[_qp] * _test[_i][_qp] *  _du_dot_du[_qp] * _phi[_j][_qp];
}