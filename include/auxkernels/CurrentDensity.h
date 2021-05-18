#pragma once

#include "AuxKernel.h"

/**
 * Deprecated auxiliary kernel responsible for computing the
 * current density given the magnetic field.
 * Deprecated in favour of Vector Curl or AVCurrentDensity.
 */
class CurrentDensity : public VectorAuxKernel
{
public:
  static InputParameters validParams();

  CurrentDensity(const InputParameters & parameters);

protected:
  /**
   * AuxKernels MUST override computeValue.  computeValue() is called on
   * every quadrature point.  For Nodal Auxiliary variables those quadrature
   * points coincide with the nodes.
   */
  virtual RealVectorValue computeValue() override;

  /// Holds the  local current density at the quadrature points
  const VectorVariableCurl & _current_density;
};
