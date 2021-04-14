//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "AuxKernel.h"

/**
 * Auxiliary kernel responsible for computing the Lorentz force per unit volume
 * given the local current density J and magnetic flux density B.
 */
class LorentzForce : public VectorAuxKernel
{
public:
  static InputParameters validParams();

  LorentzForce(const InputParameters & parameters);

protected:
  /**
   * AuxKernels MUST override computeValue.  computeValue() is called on
   * every quadrature point.  For Nodal Auxiliary variables those quadrature
   * points coincide with the nodes.
   */
  virtual RealVectorValue computeValue() override;

  /// Holds the local current density at the quadrature points
  const VectorVariableValue & _j_field;
  /// Holds the local magnetic flix density density at the quadrature points
  const VectorVariableValue & _b_field;
};
