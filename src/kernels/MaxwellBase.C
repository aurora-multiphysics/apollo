//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

//* Base class for solution of (real) Maxwell equations in time domain.
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

#include "MaxwellBase.h"
#include "Function.h"
#include "Assembly.h"

registerMooseObject("ApolloApp", MaxwellBase);

InputParameters
MaxwellBase::validParams()
{
  InputParameters params = VectorTimeKernel::validParams();
  params.addClassDescription("This class computes various components of the"
                             "Maxwell equations which can then be assembled "
                             "together in child classes.");
  params.addParam<bool>("quasistationary", true, "Whether the term proportional to the second order time derivative of the field should be neglected."
  "If false, a time integrator such as NewmarkBeta that calculates second order time derivatives must be used.");
  params.addParam<bool>("gauge_penalty",
                        false,
                        "Whether a gauge penalty term should be included for regularisation.");                             
  
  return params;
}

MaxwellBase::MaxwellBase(const InputParameters & parameters)
  : VectorTimeKernel(parameters),
    _curl_phi(_assembly.curlPhi(_var)),
    _curl_test(_var.curlPhi()),
    _curl_u(_is_implicit ? _var.curlSln() : _var.curlSlnOld()),
    _u_dotdot(getParam<bool>("quasistationary") ?   _vector_zero :_var.uDotDot()),
    _du_dotdot_du(getParam<bool>("quasistationary") ?  _zero : _var.duDotDotDu()),
    _quasistationary(getParam<bool>("quasistationary")),
    _gauge_penalty(getParam<bool>("gauge_penalty"))
{
}

Real
MaxwellBase::curlCurlTerm()
{
  return _curl_u[_qp] * _curl_test[_i][_qp];
}

Real
MaxwellBase::dCurlCurlDU()
{
  return _curl_phi[_j][_qp] * _curl_test[_i][_qp];
}

Real
MaxwellBase::gaugePenaltyTerm()
{
  return _grad_u[_qp].tr() * _grad_test[_i][_qp].tr();
}

Real
MaxwellBase::dGaugePenaltyDU()
{
  return _grad_phi[_j][_qp].tr() * _grad_test[_i][_qp].tr();
}

Real
MaxwellBase::steadyStateTerm()
{
  return curlCurlTerm() + (_gauge_penalty ? gaugePenaltyTerm() : 0);
}

Real
MaxwellBase::dSteadyStateTermDU()
{
  return dCurlCurlDU() + (_gauge_penalty ? dGaugePenaltyDU() : 0);
}

Real
MaxwellBase::firstOrderTimeDerivTerm()
{
  return _test[_i][_qp] * _u_dot[_qp];
}

Real
MaxwellBase::dFirstOrderTimeDerivDU()
{
  return _test[_i][_qp] * _du_dot_du[_qp] * _phi[_j][_qp];
}

Real
MaxwellBase::secondOrderTimeDerivTerm()
{
  return _test[_i][_qp] * _u_dotdot[_qp];
}

Real
MaxwellBase::dSecondOrderTimeDerivDU()
{
  return _test[_i][_qp] *  _du_dotdot_du[_qp] * _phi[_j][_qp];
}

Real
MaxwellBase::computeQpResidual() {return 0;}

Real
MaxwellBase::computeQpJacobian(){return 0;}