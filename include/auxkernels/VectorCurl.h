#pragma once

#include "AuxKernel.h"

/**
 * Auxiliary kernel responsible for returning the curl of a given vector field.
 */
class VectorCurl : public VectorAuxKernel
{
public:
  static InputParameters validParams();

  VectorCurl(const InputParameters & parameters);

protected:
  /**
   * AuxKernels MUST override computeValue.  computeValue() is called on
   * every quadrature point.  For Nodal Auxiliary variables those quadrature
   * points coincide with the nodes.
   */
  virtual RealVectorValue computeValue() override;

  /// Holds the  local vector curl at the quadrature points
  const VectorVariableCurl & _vector_curl;
};
