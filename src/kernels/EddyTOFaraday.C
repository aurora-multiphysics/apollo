//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

//* Solves:
//* EddyTOFaraday: ∇×(ρ∇×T) - ∇(ρ∇·T) + μd/dt(T-∇ω) = 0
//* EddyTOGauss:   ∇·(μd/dt(T-∇ω))=0
//*
//* in weak form
//* EddyTOFaraday:
//* (ρ∇×T, ∇×v) + (ρ∇·T, ∇·v) + (μd/dt(T-∇ω), v)
//* - <(ρ∇×T)×n, v> - <ρ∇·T, v·n>  = 0
//* EddyTOGauss:
//* (μd/dt(T-∇ω), ∇w) - <μd/dt(T-∇ω)·n, w> =0
//*
//* where:
//* Permeability μ
//* Resistivity ρ=1/σ
//* Electric vector potential T
//* Magnetic scalar potential ω
//* Electric field, E = ρ∇×T
//* Magnetic flux density, B = μ(T-∇ω)
//* Magnetic field H = T-∇ω
//* Current density J = ∇×T
//*
//* Natural boundary conditions are Neumann conditions setting
//* E×n (for T) and B·n (for ω) at surface, along with a gauge constraint.
//* J·n and H×n at surface imposed with Dirichlet conditions.

#include "EddyTOFaraday.h"
#include "Function.h"
#include "Assembly.h"

registerMooseObject("ApolloApp", EddyTOFaraday);

InputParameters
EddyTOFaraday::validParams()
{
  InputParameters params = MaxwellBase::validParams();
  params.addCoupledVar("scalar_potential", 0, "Scalar potential");
  return params;
}

EddyTOFaraday::EddyTOFaraday(const InputParameters & parameters)
  : MaxwellBase(parameters),
    _grad_v(coupledGradient("scalar_potential")),
    _v_id(coupled("scalar_potential")),
    _v_var(*getVar("scalar_potential", 0)),
    _standard_grad_phi(_assembly.gradPhi(_v_var)),
    _rho(getMaterialProperty<Real>("resistivity")),
    _mu(getMaterialProperty<Real>("permeability"))
{
}

Real
EddyTOFaraday::computeQpResidual()
{
  return _rho[_qp] * MaxwellBase::steadyStateTerm() +
         _mu[_qp] * (MaxwellBase::firstOrderTimeDerivTerm() - _test[_i][_qp] * _grad_v[_qp]);
}

Real
EddyTOFaraday::computeQpJacobian()
{
  return _rho[_qp] * MaxwellBase::dSteadyStateTermDU() +
         _mu[_qp] * MaxwellBase::dFirstOrderTimeDerivDU();
}

Real
EddyTOFaraday::computeQpOffDiagJacobian(unsigned jvar)
{
  if (jvar == _v_id)
    return -_test[_i][_qp] * _mu[_qp] * _standard_grad_phi[_j][_qp];
  else
    return 0.;
}