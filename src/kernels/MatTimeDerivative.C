//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "MatTimeDerivative.h"

registerMooseObject("ApolloApp", MatTimeDerivative);

defineLegacyParams(MatTimeDerivative);

InputParameters
MatTimeDerivative::validParams()
{
  InputParameters params = TimeDerivative::validParams();
  params.addParam<MaterialPropertyName>("material_property",
                                        "the name of the material property to multiply the time derivative by");
  return params;
}

MatTimeDerivative::MatTimeDerivative(const InputParameters & parameters)
  : TimeDerivative(parameters), _sigma(getMaterialProperty<Real>("material_property"))
{
}

Real
MatTimeDerivative::computeQpResidual()
{
  return _sigma[_qp] * TimeDerivative::computeQpResidual();
}

Real
MatTimeDerivative::computeQpJacobian()
{
  return _sigma[_qp] * TimeDerivative::computeQpJacobian();
}
