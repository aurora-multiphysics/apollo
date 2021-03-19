//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
//*
#pragma once

#include "TimeKernel.h"
#include "MaterialProperty.h"

class EddyAVGauss : public TimeKernel
{
public:
  static InputParameters validParams();

  EddyAVGauss(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
  virtual Real computeQpOffDiagJacobian(unsigned int jvar) override;
  const unsigned int _v_id;
  // const VectorVariableValue & _v;
  const VectorVariableValue & _v_dot;
  const VariableValue & _dv_dot_dv;
  VectorMooseVariable & _v_var;  
  const VectorVariablePhiValue & _vector_phi;
  const MaterialProperty<Real> & _sigma;
  Real _sgn;
};
