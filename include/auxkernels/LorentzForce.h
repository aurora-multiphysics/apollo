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
  /// Holds the local magnetic flux density density at the quadrature points
  const VectorVariableValue & _b_field;
};
