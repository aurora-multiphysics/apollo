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

#include "VectorTimeKernel.h"
#include "MaterialProperty.h"

class ExplicitUngaugedMaxwell : public VectorTimeKernel
{
public:
  static InputParameters validParams();

  ExplicitUngaugedMaxwell(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
  /// curl of the test function
  const VectorVariableTestCurl & _curl_test;

  /// curl of the shape function
  const VectorVariablePhiCurl & _curl_phi;

  /// Holds the solution curl at the current quadrature points
  const VectorVariableCurl & _curl_u;

  /// Time derivative of u
  const VectorVariableValue & _u_dot;

  /// Derivative of u_dot with respect to u
  const VariableValue & _du_dot_du;

  /// The resistivity at the current quadrature point. See MatDiffusionBase
  const MaterialProperty<Real> & _jc;
  const MaterialProperty<Real> & _n;
  const MaterialProperty<Real> & _rho;
  const MaterialProperty<Real> & _mu;
};
