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

//* For T-phi formulation, u = T, p = omega, a = rho, s = mu
//* and H = T0 + T - grad phi = u + grad p

//* For A-V formulation, u = A, p = int(V dt), a = mu^-1, s = sigma
//* B = curl A

#include "GaugedMaxwell.h"
#include "Function.h"
#include "Assembly.h"

registerMooseObject("ApolloApp", GaugedMaxwell);

InputParameters
GaugedMaxwell::validParams()
{
  InputParameters params = VectorTimeKernel::validParams();
  params.addParam<FunctionName>("x_forcing_func", 0, "The x forcing function.");
  params.addParam<FunctionName>("y_forcing_func", 0, "The y forcing function.");
  params.addParam<FunctionName>("z_forcing_func", 0, "The z forcing function.");
  params.addCoupledVar("scalar_potential", 0, "Scalar potential");
  return params;
}

GaugedMaxwell::GaugedMaxwell(const InputParameters & parameters)
  : VectorTimeKernel(parameters),
    _curl_phi(_assembly.curlPhi(_var)),
    _curl_test(_var.curlPhi()),
    _curl_u(_is_implicit ? _var.curlSln() : _var.curlSlnOld()),
    _u_dot(_var.uDot()),
    _du_dot_du(_var.duDotDu()),
    _grad_v_dot(coupledGradientDot("scalar_potential")),
    _dv_dot_dv(coupledDotDu("scalar_potential")),
    _v_id(coupled("scalar_potential")),
    _v_var(*getVar("scalar_potential", 0)),
    _standard_grad_phi(_assembly.gradPhi(_v_var)),
    // _curl_u(coupledCurl(_var)),
    _x_ffn(getFunction("x_forcing_func")),
    _y_ffn(getFunction("y_forcing_func")),
    _z_ffn(getFunction("z_forcing_func")),
    _resistivity(getMaterialProperty<Real>("resistivity")),
    _drhodj(getMaterialProperty<Real>("drhodj")),
    _permeability(getMaterialProperty<Real>("permeability"))
{
  // use component-wise curl on phi, u and test?
}

Real
GaugedMaxwell::computeQpResidual()
{
  return _resistivity[_qp] * _curl_u[_qp] * _curl_test[_i][_qp] +
         _resistivity[_qp] * _grad_u[_qp].tr() * _grad_test[_i][_qp].tr() -
         RealVectorValue(_x_ffn.value(_t, _q_point[_qp]),
                         _y_ffn.value(_t, _q_point[_qp]),
                         _z_ffn.value(_t, _q_point[_qp])) *
             _test[_i][_qp] +
         _test[_i][_qp] * _permeability[_qp] * (_u_dot[_qp] + _grad_v_dot[_qp]);
}

Real
GaugedMaxwell::computeQpJacobian()
{
  return _drhodj[_qp] * _curl_phi[_j][_qp] * _curl_test[_i][_qp] +
         _drhodj[_qp] * _grad_phi[_j][_qp].tr() * _grad_test[_i][_qp].tr() +
         _test[_i][_qp] * _permeability[_qp] * _du_dot_du[_qp] * _phi[_j][_qp];
}

Real
GaugedMaxwell::computeQpOffDiagJacobian(unsigned jvar)
{
  if (jvar == _v_id)
    return _test[_i][_qp] * _permeability[_qp] * _dv_dot_dv[_qp] * _standard_grad_phi[_j][_qp];
  else
    return 0.;
}