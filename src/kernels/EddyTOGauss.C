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

#include "EddyTOGauss.h"
#include "Function.h"
#include "Assembly.h"

registerMooseObject("ApolloApp", EddyTOGauss);

InputParameters
EddyTOGauss::validParams()
{
  InputParameters params = TimeKernel::validParams();
  params.addParam<FunctionName>("x_forcing_func", 0, "The x forcing function.");
  params.addParam<FunctionName>("y_forcing_func", 0, "The y forcing function.");
  params.addParam<FunctionName>("z_forcing_func", 0, "The z forcing function.");
  params.addCoupledVar("vector_potential", 0, "Vector potential");
  return params;
}

EddyTOGauss::EddyTOGauss(const InputParameters & parameters)
  : TimeKernel(parameters),
    _grad_u_dot(_var.gradSlnDot()),
    _du_dot_du(_var.duDotDu()),
    _v_id(coupled("vector_potential")),
    _v_dot(coupledVectorDot("vector_potential")),
    _dv_dot_dv(coupledVectorDotDu("vector_potential")),
    _v_var(*getVectorVar("vector_potential", 0)),
    _vector_phi(_assembly.phi(_v_var)),
    _mu(getMaterialProperty<Real>("permeability"))
{
}

Real
EddyTOGauss::computeQpResidual()
{
  return _grad_test[_i][_qp] * _mu[_qp] * (_grad_u_dot[_qp] - _v_dot[_qp]);
}

Real
EddyTOGauss::computeQpJacobian()
{
  return _grad_test[_i][_qp] * _mu[_qp] * _du_dot_du[_qp] * _grad_phi[_j][_qp];
}

Real
EddyTOGauss::computeQpOffDiagJacobian(unsigned int jvar)
{
  if (jvar == _v_id)
    return -_grad_test[_i][_qp] * _mu[_qp] * _dv_dot_dv[_qp] * _vector_phi[_j][_qp];
  else
    return 0;
}