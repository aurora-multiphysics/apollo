#pragma once

#include "AuxKernel.h"

/**
 * Auxiliary kernel responsible for computing the magnitude of
 * a complex vector given the real and imaginary components.
 */
class ComplexMagnitude : public VectorAuxKernel
{
public:
  static InputParameters validParams();

  ComplexMagnitude(const InputParameters & parameters);

protected:
  /**
   * AuxKernels MUST override computeValue.  computeValue() is called on
   * every quadrature point.  For Nodal Auxiliary variables those quadrature
   * points coincide with the nodes.
   */
  virtual RealVectorValue computeValue() override;

  /// Vectors holding the real and imaginary components of the complex variable.
  const VectorVariableValue & _vec_real;
  const VectorVariableValue & _vec_imag;
};
