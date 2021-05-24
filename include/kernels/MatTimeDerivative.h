//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "TimeDerivative.h"
#include "MaterialProperty.h"

class MatTimeDerivative;

template <>
InputParameters validParams<MatTimeDerivative>();

/**
 * Time derivative term multiplied by a material coefficient
 */
class MatTimeDerivative : public TimeDerivative
{
public:
  static InputParameters validParams();

  MatTimeDerivative(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();

  /// The coefficient the time derivative is multiplied with
  const MaterialProperty<Real> & _sigma;
};

