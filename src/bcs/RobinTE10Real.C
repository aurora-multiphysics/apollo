//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
//* Equivalent to VectorCurlPenalty with penalty = gamma/mu_0, u_exact = -2E_10 or 0
//* Variable should be IMAGINARY
#include "RobinTE10Real.h"
#include "Function.h"

registerMooseObject("HammerheadApp", RobinTE10Real);

InputParameters
RobinTE10Real::validParams()
{
  InputParameters params = VectorIntegratedBC::validParams();
  params.addParam<FunctionName>("E10_re", 0, "The real component of the electric field at the boundary");
  params.addParam<FunctionName>("E10_im", 0, "The imaginary component of the electric field at the boundary");  
  params.addParam<FunctionName>("robin_im", 0, "The imaginary component of the propogation constant at the boundary");  
  params.addParam<bool>("input_port", false, "Whether the boundary is being applied on the input port.");
  return params;
}

RobinTE10Real::RobinTE10Real(const InputParameters & parameters)
  : VectorIntegratedBC(parameters),
    _E10_re(isParamValid("E10_re") ? &getFunction("function") : nullptr),
    _E10_im(isParamValid("E10_im") ? &getFunction("function") : nullptr),
    _gamma_im(isParamValid("robin_im") ? &getFunction("function") : nullptr),
    _input_port(getParam<bool>("input_port"))
{
}

Real
RobinTE10Real::computeQpResidual()
{
  Real mu0 = 1.25663706e-6;
  RealVectorValue u_exact_re;
  RealVectorValue u_exact_im;
  if (_input_port)
  {
    u_exact_re = 2*_E10_re->vectorValue(_t, _q_point[_qp]);
    u_exact_im = 2*_E10_im->vectorValue(_t, _q_point[_qp]);
  }else{
    u_exact_re = RealVectorValue(0, 0, 0);
    u_exact_im = RealVectorValue(0, 0, 0);
  }RealVectorValue Ncu_re = -_gamma_im->value(_t, _q_point[_qp])* (_u[_qp] - u_exact_im).cross(_normals[_qp]);
  // RealVectorValue Ncu_im = _gamma_im* (_u[_qp] - u_exact_re).cross(_normals[_qp]);
  return  (1/mu0) * Ncu_re * ((_test[_i][_qp]).cross(_normals[_qp]));
}

Real
RobinTE10Real::computeQpJacobian()
{
  Real mu0 = 1.25663706e-6;
  return -(1/mu0) * _gamma_im->value(_t, _q_point[_qp]) * (_phi[_j][_qp]).cross(_normals[_qp]) * (_test[_i][_qp]).cross(_normals[_qp]);
}
