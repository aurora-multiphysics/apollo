//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

//* Solves:
//* EddyAVFaraday: ∇×(ν∇×A) - ∇(ν∇·A) + σ(dA/dt + ∇ V) = 0
//* EddyAVGauss:   ∇·(σ(dA/dt + ∇ V))= 0
//*
//* in weak form
//* EddyAVFaraday:
//* (ν∇×A, ∇×v) + (ν∇·A, ∇·v) + (σ(dA/dt + ∇ V), v)
//* - <(ν∇×A) × n, v> - <ν∇·A, v·n>  = 0
//* EddyAVGauss:
//* (σ(dA/dt + ∇ V), ∇w) - <σ(dA/dt + ∇ V)·n, w> =0
//*
//* where:
//* reluctivity ν = 1/μ
//* electrical_conductivity σ=1/ρ
//* Magnetic vector potential A
//* Scalar electric potential V
//* Electric field, E = -dA/dt -∇V
//* Magnetic flux density, B = ∇×A
//* Magnetic field H = ν∇×A
//* Current density J = -σ(dA/dt + ∇ V)
//*

//* Either:
//* set B.n (or E×n) at boundary: A×n (Dirichlet), ν∇·A (Neumann)
//* set H×n at boundary: ν∇×A (Neumann), A·n (Dirichlet)
//* Scalar V: -σ(dA/dt + ∇ V)·n (J·n, Neumann), V (potential, Dirichlet)
//* Natural boundary conditions:
//* setting on H×n and J·n at surface, along with a gauge constraint.
//* B.n and E×n at surface imposed with Dirichlet conditions.

#include "EddyAVFaraday.h"
#include "Function.h"
#include "Assembly.h"

registerMooseObject("ApolloApp", EddyAVFaraday);

InputParameters
EddyAVFaraday::validParams()
{
  InputParameters params = MaxwellBase::validParams();
  params.addCoupledVar("scalar_potential", 0, "Scalar potential");
  return params;
}

EddyAVFaraday::EddyAVFaraday(const InputParameters & parameters)
  : MaxwellBase(parameters),
    _grad_v(coupledGradient("scalar_potential")),
    _v_id(coupled("scalar_potential")),
    _v_var(*getVar("scalar_potential", 0)),
    _standard_grad_phi(_assembly.gradPhi(_v_var)),
    _nu(getMaterialProperty<Real>("reluctivity")),
    _sigma(getMaterialProperty<Real>("electrical_conductivity"))
{
}

Real
EddyAVFaraday::computeQpResidual()
{
  return _nu[_qp] * MaxwellBase::steadyStateTerm() +
         _sigma[_qp] * (MaxwellBase::firstOrderTimeDerivTerm() + _test[_i][_qp] * _grad_v[_qp]);
}

Real
EddyAVFaraday::computeQpJacobian()
{
  return _nu[_qp] * MaxwellBase::dSteadyStateTermDU() +
         _sigma[_qp] * MaxwellBase::dFirstOrderTimeDerivDU();
}

Real
EddyAVFaraday::computeQpOffDiagJacobian(unsigned jvar)
{
  if (jvar == _v_id)
    return _test[_i][_qp] * _sigma[_qp] * _standard_grad_phi[_j][_qp];
  else
    return 0.;
}