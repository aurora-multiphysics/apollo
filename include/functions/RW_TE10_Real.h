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

class RW_TE10_Real;

template <>
InputParameters validParams<RW_TE10_Real>();

class RW_TE10_Real : public Function
{
public:
  RW_TE10_Real(const InputParameters & parameters);

  virtual RealVectorValue vectorValue(Real t, const Point & p) const override;
  Real const _a;
  Real const _b;
  Real const _omega;
protected:

};
