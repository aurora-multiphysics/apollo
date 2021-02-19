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
#include "WaveguideProperties.h"
class RW_TE10_Imag;

template <>
InputParameters validParams<RW_TE10_Imag>();

class RW_TE10_Imag : public Function
{
public:
  RW_TE10_Imag(const InputParameters & parameters);

  virtual RealVectorValue vectorValue(Real t, const Point & p) const override;

protected:
  const WaveguideProperties & _wp;
};
