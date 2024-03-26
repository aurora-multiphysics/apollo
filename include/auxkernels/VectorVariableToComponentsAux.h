#pragma once

#include "WritableVectorAuxKernel.h"

/**
 * Set 3 standard variables from a vector variable.
 */
class VectorVariableToComponentsAux : public WritableVectorAuxKernel
{
public:
  static InputParameters validParams();

  VectorVariableToComponentsAux(const InputParameters & parameters);

  void compute() override;

protected:
  /**
   * Writable references to component variables.
   */
  MooseVariable & _component_x;
  MooseVariable & _component_y;
  MooseVariable & _component_z;
};
