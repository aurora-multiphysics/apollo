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

#include "MaxwellH.h"
#include "Function.h"
#include "Assembly.h"

registerMooseObject("ApolloApp", MaxwellH);

InputParameters
MaxwellH::validParams()
{
  InputParameters params = MaxwellBase::validParams();
  return params;
}

MaxwellH::MaxwellH(const InputParameters & parameters)
  : MaxwellBase(parameters),
    _rho(getMaterialProperty<Real>("resistivity")),
    _mu(getMaterialProperty<Real>("permeability"))
{
}

Real
MaxwellH::computeQpResidual()
{
  return _rho[_qp] * MaxwellBase::steadyStateTerm()
         + _mu[_qp] * MaxwellBase::firstOrderTimeDerivTerm();
}

Real
MaxwellH::computeQpJacobian()
{
  return _rho[_qp] * MaxwellBase::dSteadyStateTermDU() +
         _mu[_qp] *  MaxwellBase::dFirstOrderTimeDerivDU();
}