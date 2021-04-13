//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "Function.h"

class TE10Real;

template <>
InputParameters validParams<Function>();

/**
 * Class that represents constant function
 */
class TE10Real : public Function
{
public:
  static InputParameters validParams();

  TE10Real(const InputParameters & parameters);

  virtual RealVectorValue vectorValue(Real t, const Point & p) const override;

protected:
  Real kc;
  Real gamma_im;
  Real E0;
  MooseEnum propagation_direction;
};
