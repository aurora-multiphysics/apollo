//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "AVCurrentFlowBC.h"
#include "Function.h"

registerMooseObject("ApolloApp", AVCurrentFlowBC);

InputParameters
AVCurrentFlowBC::validParams()
{
  InputParameters params = VectorIntegratedBC::validParams();
  params.addRequiredParam<FunctionName>("curl_value", "Value of field curl on the boundary.");
  return params;
}

AVCurrentFlowBC::AVCurrentFlowBC(const InputParameters & parameters)
  : VectorIntegratedBC(parameters), _curl_value(getFunction("curl_value"))
{
}

Real
AVCurrentFlowBC::computeQpResidual()
{
  return _test[_i][_qp] * _normals[_qp].cross(_curl_value.vectorCurl(_t, _q_point[_qp]));
}

Real
AVCurrentFlowBC::computeQpJacobian()
{
  return 0.0;
}
