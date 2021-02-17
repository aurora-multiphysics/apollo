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

class PropagationConstant_Imag;

template <>
InputParameters validParams<PropagationConstant_Imag>();

class PropagationConstant_Imag : public Function
{
public:
  PropagationConstant_Imag(const InputParameters & parameters);

  virtual Real value(Real t, const Point & p) const override;
  Real const _a;
  Real const _b;
  Real const _omega;
protected:
};
