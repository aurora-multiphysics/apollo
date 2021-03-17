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

class AV_Faraday : public VectorTimeKernel
{
public:
  static InputParameters validParams();

  AV_Faraday(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
  virtual Real computeQpOffDiagJacobian(unsigned int jvar) override;
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

  const VariableGradient & _grad_v;
  const unsigned _v_id;
  MooseVariable & _v_var;
  const VariablePhiGradient & _standard_grad_phi;

  const MaterialProperty<Real> & _rho;
  const MaterialProperty<Real> & _mu;

  /// The resistivity at the current quadrature point.
  // const MaterialProperty<Real> & _permeability;
  // const MaterialProperty<Real> & _resistivity;
  // const MaterialProperty<Real> & _drhodj;
};
