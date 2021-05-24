#pragma once

#include "AuxKernel.h"

/**
 * Auxiliary kernel responsible for computing the current density given
 * the magnetic vector potential A and the electric scalar potential V.
 */
class A2DMagneticField : public VectorAuxKernel
{
public:
  static InputParameters validParams();

  A2DMagneticField(const InputParameters & parameters);

protected:
  /**
   * AuxKernels MUST override computeValue.  computeValue() is called on
   * every quadrature point.  For Nodal Auxiliary variables those quadrature
   * points coincide with the nodes.
   */
  virtual RealVectorValue computeValue() override;

  /// Holds the  local current density at the quadrature points
  const VariableGradient & _grad_a;
  const MaterialProperty<Real> & _sigma;
};
