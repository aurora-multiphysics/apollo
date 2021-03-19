//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
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

// aux div u = 0 with u = B or J
//* For T-phi formulation, u = T, p = omega, a = rho, s = mu
//* and H = T0 + T - grad phi = u + grad p

//* For A-V formulation, u = A, p = int(V dt), a = mu^-1, s = sigma
//* B = curl A

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
    _permeability(getMaterialProperty<Real>("permeability"))
{
  // use component-wise curl on phi, u and test?
}

Real
EddyAVGauss::computeQpResidual()
{
  return _grad_test[_i][_qp] * _permeability[_qp] * (_grad_u[_qp] + _v_dot[_qp]);
}
// Jc(B) implemented like ffn ->
Real
EddyAVGauss::computeQpJacobian()
{
  return _grad_test[_i][_qp] * _permeability[_qp] * _grad_phi[_j][_qp];
}

Real
EddyAVGauss::computeQpOffDiagJacobian(unsigned int jvar)
{
  if (jvar == _v_id)
    // return _grad_test[_i][_qp] * _permeability[_qp] * _vector_phi[_j][_qp];
    return _grad_test[_i][_qp] * _permeability[_qp] * _dv_dot_dv[_qp] * _vector_phi[_j][_qp];
  else
    return 0;
}