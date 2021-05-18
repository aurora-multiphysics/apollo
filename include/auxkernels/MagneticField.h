#pragma once

#include "AuxKernel.h"

/**
 * Auxiliary kernel responsible for computing the Darcy velocity given
 * several fluid properties and the pressure gradient.
 */
class MagneticField : public VectorAuxKernel
{
public:
  static InputParameters validParams();

  MagneticField(const InputParameters & parameters);

protected:
  /**
   * AuxKernels MUST override computeValue.  computeValue() is called on
   * every quadrature point.  For Nodal Auxiliary variables those quadrature
   * points coincide with the nodes.
   */
  virtual RealVectorValue computeValue() override;

  /// Holds the  local current density at the quadrature points
  const VectorVariableValue & _T;
  const VariableGradient & _grad_omega;
};
