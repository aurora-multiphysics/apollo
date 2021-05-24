#pragma once

#include "AuxKernel.h"

/**
 * Auxiliary kernel responsible for computing the current density given
 * the magnetic vector potential A and the electric scalar potential V.
 */
class A2DCurrentDensity : public AuxKernel
{
public:
  static InputParameters validParams();

  A2DCurrentDensity(const InputParameters & parameters);

protected:
  /**
   * AuxKernels MUST override computeValue.  computeValue() is called on
   * every quadrature point.  For Nodal Auxiliary variables those quadrature
   * points coincide with the nodes.
   */
  virtual Real computeValue() override;

  /// Holds the  local current density at the quadrature points
  const VariableValue & _a_dot;
  const MaterialProperty<Real> & _sigma;
};
