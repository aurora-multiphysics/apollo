#pragma once

#include "WritableVectorAuxKernel.h"

/**
 * Construct a vector variable from 3 standard variables.
 */
class VectorVariableFromComponentsAux : public WritableVectorAuxKernel
{
public:
  static InputParameters validParams();

  VectorVariableFromComponentsAux(const InputParameters & parameters);

  void compute() override;

protected:
  /**
   * Non-writable references to the component variables.
   */
  const MooseVariable & _component_x;
  const MooseVariable & _component_y;
  const MooseVariable & _component_z;
};
