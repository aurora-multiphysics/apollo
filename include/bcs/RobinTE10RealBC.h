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
#include "WaveguideProperties.h"
class RobinTE10RealBC : public VectorIntegratedBC
{
public:
  static InputParameters validParams();

  RobinTE10RealBC(const InputParameters & parameters);

protected:
  Real _penalty;
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  // Holds the solution curl at the current quadrature points
  const VectorVariableValue & _v;
  const WaveguideProperties & _wp;
  bool _input_port;
};
