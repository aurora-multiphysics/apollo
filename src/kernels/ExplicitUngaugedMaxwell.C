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
//* (xi * curl u, curl v) + (xi * div u, div v) + (d/dt eta*( u + grad p), v)
//* - <(xi*curl u) x n, v> - <xi*div u, v.n>  = 0
//* (d/dt eta*( u + grad p), grad w) - <d/dt eta*( u + grad p) . n, w> =0

//* For T-phi formulation, u = T, p = omega, xi = rho, eta = mu
//* and H = T0 + T - grad phi = u + grad p

//* For A-V formulation, u = A, p = int(V dt), xi = mu^-1, eta = sigma
//* B = curl A

#include "ExplicitUngaugedMaxwell.h"
#include "Function.h"
#include "Assembly.h"

registerMooseObject("ApolloApp", ExplicitUngaugedMaxwell);

InputParameters
ExplicitUngaugedMaxwell::validParams()
{
  InputParameters params = VectorTimeKernel::validParams();
  return params;
}

ExplicitUngaugedMaxwell::ExplicitUngaugedMaxwell(const InputParameters & parameters)
  : VectorTimeKernel(parameters),
    _curl_phi(_assembly.curlPhi(_var)),
    _curl_test(_var.curlPhi()),
    _curl_u(_is_implicit ? _var.curlSln() : _var.curlSlnOld()),
    _u_dot(_var.uDot()),
    // _has_horiz_num(isParamValid("horizon_number")),
    _du_dot_du(_var.duDotDu()),
    _jc(getMaterialProperty<Real>("critical_current_density")),
    _n(getMaterialProperty<Real>("nonlinearity_parameter")),
    _rho(getMaterialProperty<Real>("resistivity")),
    _mu(getMaterialProperty<Real>("permeability"))
{
}
// xi = E/J = rho
// dxi du = dE/dJ = J drdJ +
Real
ExplicitUngaugedMaxwell::computeQpResidual()
{
  return _rho[_qp] * _curl_u[_qp] * _curl_test[_i][_qp] + _test[_i][_qp] * _mu[_qp] * _u_dot[_qp];
}

Real
ExplicitUngaugedMaxwell::computeQpJacobian()
{
  return _rho[_qp] * _curl_phi[_j][_qp] * _curl_test[_i][_qp] +
         _test[_i][_qp] * _mu[_qp] * _du_dot_du[_qp] * _phi[_j][_qp] +
         (_n[_qp] - 1) * _rho[_qp] * (1 / (1e-99 + pow((_curl_u[_qp].norm()), 2))) *
             (1e-99 + ((_curl_phi[_j][_qp].contract(_curl_u[_qp]))) *
                          ((_curl_u[_qp].contract(_curl_test[_i][_qp]))));
}
