#pragma once

#include "AuxKernel.h"

/**
 * Auxiliary kernel responsible for computing the current density given
 * the magnetic vector potential A and the electric scalar potential V.
 */
class A2DLorentzForce : public VectorAuxKernel
{
public:
  static InputParameters validParams();

  A2DLorentzForce(const InputParameters & parameters);

protected:
  /**
   * AuxKernels MUST override computeValue.  computeValue() is called on
   * every quadrature point.  For Nodal Auxiliary variables those quadrature
   * points coincide with the nodes.
   */
  virtual RealVectorValue computeValue() override;

  /// Holds the  local current density at the quadrature points
  const VariableValue & _j_field;
  const VectorVariableValue & _b_field;
};
