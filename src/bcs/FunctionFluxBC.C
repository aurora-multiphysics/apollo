//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "FunctionFluxBC.h"
#include "Function.h"

registerMooseObject("ApolloApp", FunctionFluxBC);

InputParameters
FunctionFluxBC::validParams()
{
  InputParameters params = IntegratedBC::validParams();
  params.addParam<FunctionName>("function",
                                "The boundary condition vector function. This cannot be supplied "
                                "with the component parameters.");
  params.addParam<FunctionName>("function_x", 0, "The function for the x component");
  params.addParam<FunctionName>("function_y", 0, "The function for the y component");
  params.addParam<FunctionName>("function_z", 0, "The function for the z component");
  return params;
}

FunctionFluxBC::FunctionFluxBC(const InputParameters & parameters)
  : IntegratedBC(parameters),
    _function(isParamValid("function") ? &getFunction("function") : nullptr),
    _function_x(isParamValid("x_exact_soln") ? getFunction("x_exact_soln")
                                             : getFunction("function_x")),
    _function_y(isParamValid("y_exact_soln") ? getFunction("y_exact_soln")
                                             : getFunction("function_y")),
    _function_z(isParamValid("z_exact_soln") ? getFunction("z_exact_soln")
                                             : getFunction("function_z"))
{
}

Real
FunctionFluxBC::computeQpResidual()
{
  if (_function)
    _values = _function->vectorValue(_t, _q_point[_qp]);
  else
    _values = {_function_x.value(_t, _q_point[_qp]),
               _function_y.value(_t, _q_point[_qp]),
               _function_z.value(_t, _q_point[_qp])};
  return -_test[_i][_qp] * (_values * _normals[_qp]);
}