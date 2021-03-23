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
//* Reluctance ν = 1/μ
//* Conductivity σ=1/ρ
//* Magnetic vector potential A
//* Scalar electric potential V
//* Electric field, E = -dA/dt -∇V
//* Magnetic flux density, B = ∇×A
//* Magnetic field H = ν∇×A
//* Current density J = -σ(dA/dt + ∇ V)

#include "EddyAVGauss.h"
#include "Function.h"
#include "Assembly.h"

registerMooseObject("ApolloApp", EddyAVGauss);

InputParameters
EddyAVGauss::validParams()
{
  InputParameters params = TimeKernel::validParams();
  params.addParam<FunctionName>("x_forcing_func", 0, "The x forcing function.");
  params.addParam<FunctionName>("y_forcing_func", 0, "The y forcing function.");
  params.addParam<FunctionName>("z_forcing_func", 0, "The z forcing function.");
  params.addCoupledVar("vector_potential", 0, "Vector potential");
  return params;
}

EddyAVGauss::EddyAVGauss(const InputParameters & parameters)
  : TimeKernel(parameters),
    _v_id(coupled("vector_potential")),
    _v_dot(coupledVectorDot("vector_potential")),
    _dv_dot_dv(coupledVectorDotDu("vector_potential")),
    _v_var(*getVectorVar("vector_potential", 0)),
    _vector_phi(_assembly.phi(_v_var)),
    _sigma(getMaterialProperty<Real>("conductivity"))
{
}

Real
EddyAVGauss::computeQpResidual()
{
  return _grad_test[_i][_qp] * _sigma[_qp] * (_grad_u[_qp] + _v_dot[_qp]);
}

Real
EddyAVGauss::computeQpJacobian()
{
  return _grad_test[_i][_qp] * _sigma[_qp] * _grad_phi[_j][_qp];
}

Real
EddyAVGauss::computeQpOffDiagJacobian(unsigned int jvar)
{
  if (jvar == _v_id)
    return _grad_test[_i][_qp] * _sigma[_qp] * _dv_dot_dv[_qp] * _vector_phi[_j][_qp];
  else
    return 0;
}