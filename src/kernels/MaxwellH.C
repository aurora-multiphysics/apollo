//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

//* Solves Faraday's law in the quasistatic limit:
//* MaxwellH: ∇×(ρ∇×H) + μdH/dt = 0
//*
//* in weak form
//* MaxwellH:
//* (ρ∇×H, ∇×v) + (μdH/dt, v) - <(ρ∇×H)×n, v>  = 0

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