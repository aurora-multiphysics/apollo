#pragma once

#include "ApolloVectorAuxKernel.h"

/**
 * Construct a vector variable from 3 standard variables.
 */
class VectorVariableComponentsAux : public ApolloVectorAuxKernel
{
public:
  static InputParameters validParams();

  VectorVariableComponentsAux(const InputParameters & parameters);

protected:
  virtual void compute() override final;

  /**
   * The component variables. These are set internally from the vector variable.
   */
  MooseVariable & _component_x;
  MooseVariable & _component_y;
  MooseVariable & _component_z;
};
