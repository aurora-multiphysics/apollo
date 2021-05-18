#pragma once

#include "AuxKernel.h"

/**
 * Auxiliary kernel responsible for computing the electric field
 * in the AV formulation, given the magnetic vector potential
 * and electric scalar potential.
 */
class ElectricField : public VectorAuxKernel
{
public:
  static InputParameters validParams();

  ElectricField(const InputParameters & parameters);

protected:
  /**
   * AuxKernels MUST override computeValue.  computeValue() is called on
   * every quadrature point.  For Nodal Auxiliary variables those quadrature
   * points coincide with the nodes.
   */
  virtual RealVectorValue computeValue() override;

  const VectorVariableValue & _a_dot;
  const VariableGradient & _grad_v;
};
