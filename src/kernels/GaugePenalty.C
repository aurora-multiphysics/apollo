//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

//* Adds the gauge penalty term
//* - penalty * grad(xi * div u)
//*
//* in weak form:
//* + (xi * div u, div v)

//* For T-phi formulation, u = T, p = omega, xi = rho, eta = mu
//* and H = T0 + T - grad phi = u + grad p

//* For A-V formulation, u = A, p = int(V dt), xi = mu^-1, eta = sigma
//* B = curl A

#include "GaugePenalty.h"
#include "Function.h"
#include "Assembly.h"

registerMooseObject("HammerheadApp", GaugePenalty);

InputParameters
GaugePenalty::validParams()
{
  InputParameters params = VectorKernel::validParams();
  params.addCoupledVar("penalty", 1.0, "Factor to scale gauge penalty term by");
  return params;
}

GaugePenalty::GaugePenalty(const InputParameters & parameters)
  : VectorKernel(parameters),
    _curl_phi(_assembly.curlPhi(_var)),
    _curl_test(_var.curlPhi()),
    _curl_u(_is_implicit ? _var.curlSln() : _var.curlSlnOld())
{
}
// xi = E/J = rho
// dxi du = dE/dJ = J drdJ +
Real
GaugePenalty::computeQpResidual()
{
  return _grad_u[_qp].tr() * _grad_test[_i][_qp].tr();
}

Real
GaugePenalty::computeQpJacobian()
{
  return _grad_phi[_j][_qp].tr() * _grad_test[_i][_qp].tr();
}
