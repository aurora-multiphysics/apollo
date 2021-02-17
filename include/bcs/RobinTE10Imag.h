//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "VectorIntegratedBC.h"

class RobinTE10Imag : public VectorIntegratedBC
{
public:
  static InputParameters validParams();

  RobinTE10Imag(const InputParameters & parameters);

protected:
  Real _penalty;
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  const Function * const _E10_re;
  const Function * const _E10_im;
  const Function * const _gamma_im;
  bool _input_port;
};
