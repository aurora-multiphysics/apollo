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
         _mu[_qp] * (MaxwellBase::firstOrderTimeDerivTerm() + _test[_i][_qp] * _grad_v[_qp]);
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
    return _test[_i][_qp] * _mu[_qp] * _standard_grad_phi[_j][_qp];
  else
    return 0.;
}