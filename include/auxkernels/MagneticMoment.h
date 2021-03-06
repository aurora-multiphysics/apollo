#pragma once

#include "AuxKernel.h"

/**
 * Auxiliary kernel responsible for computing the magnetic moment
 * given the current density distribution.
 */
class MagneticMoment : public VectorAuxKernel
{
public:
  static InputParameters validParams();

  MagneticMoment(const InputParameters & parameters);

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
