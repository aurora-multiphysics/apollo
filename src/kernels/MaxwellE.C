//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

//* Solves time derivative of Faraday's law in the quasistatic limit:
//* ∇×(ν∇×E) + σdE/dt = 0
//*
//* in weak form
//* MaxwellE:
//* (ν∇×E, ∇×v) + (σdE/dt, v) - <(ν∇×E)×n, v> = 0

#include "MaxwellE.h"
#include "Function.h"
#include "Assembly.h"

registerMooseObject("ApolloApp", MaxwellE);

InputParameters
MaxwellE::validParams()
{
  InputParameters params = MaxwellBase::validParams();
  return params;
}

MaxwellE::MaxwellE(const InputParameters & parameters)
  : MaxwellBase(parameters), 
    _rho(getMaterialProperty<Real>("resistivity")),
    _mu(getMaterialProperty<Real>("permeability")) 
{
}

Real
MaxwellE::computeQpResidual()
{
  return (1.0/_mu[_qp]) * MaxwellBase::steadyStateTerm() +
         (1.0/_rho[_qp]) * MaxwellBase::firstOrderTimeDerivTerm();
}

Real
MaxwellE::computeQpJacobian()
{
  return (1.0/_mu[_qp]) * MaxwellBase::dSteadyStateTermDU() +
         (1.0/_rho[_qp]) * MaxwellBase::dFirstOrderTimeDerivDU();
}