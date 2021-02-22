//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
//* Equivalent to VectorCurlPenalty with penalty = gamma/mu_0, u_exact = -2E_10 or 0
//* Variable should be REAL
#include "RobinTE10ImagBC.h"
#include "Function.h"

registerMooseObject("HammerheadApp", RobinTE10ImagBC);

InputParameters
RobinTE10ImagBC::validParams()
{
  InputParameters params = VectorIntegratedBC::validParams();
  params.addRequiredParam<UserObjectName>("waveguide_properties",
                                          "The name of the user object for waveguide properties");  
  params.addParam<bool>("input_port", false, "Whether the boundary is being applied on the input port.");
  params.addRequiredCoupledVar("v", "Coupled vector variable");
  return params;
}

RobinTE10ImagBC::RobinTE10ImagBC(const InputParameters & parameters)
  : VectorIntegratedBC(parameters),
    _v(coupledVectorValue("v")),
    _wp(getUserObject<WaveguideProperties>("waveguide_properties")),
    _input_port(getParam<bool>("input_port"))
{
}

Real
RobinTE10ImagBC::computeQpResidual()
{
  RealVectorValue u_exact_re;
  RealVectorValue u_exact_im;
  if (_input_port)
  {
    u_exact_re = 2*_wp.getRealRWTE10(_t, _q_point[_qp]);
    u_exact_im = 2*_wp.getImagRWTE10(_t, _q_point[_qp]);
  }else{
    u_exact_re = RealVectorValue(0, 0, 0);
    u_exact_im = RealVectorValue(0, 0, 0);
  }
  // RealVectorValue Ncu_re = -_gamma_im->value(_t, _q_point[_qp])* (_u[_qp] - u_exact_im).cross(_normals[_qp]);
  RealVectorValue Ncu_im = _wp.getImagPropagationConstant()* (_v[_qp] - u_exact_re).cross(_normals[_qp]);
  return  (1/_wp._mu0) * Ncu_im * ((_test[_i][_qp]).cross(_normals[_qp]));
}

Real
RobinTE10ImagBC::computeQpJacobian()
{
  return (1/_wp._mu0) * _wp.getImagPropagationConstant() * (_phi[_j][_qp]).cross(_normals[_qp]) * (_test[_i][_qp]).cross(_normals[_qp]);
}
