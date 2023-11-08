#pragma once

#include "ApolloVectorAuxKernel.h"

/**
 * Construct a vector variable from 3 standard variables.
 */
class VectorVariableFromComponentsAux : public ApolloVectorAuxKernel
{
public:
  static InputParameters validParams();

  VectorVariableFromComponentsAux(const InputParameters & parameters);

protected:
  virtual void compute() override final;

  const VariableValue & _component_x;
  const VariableValue & _component_y;
  const VariableValue & _component_z;

  const Order & _vector_order;
  const FEFamily _vector_family;

private:
  void checkVectorVariable() const;
  void checkVectorComponents() const;
  void checkVectorComponent(const MooseVariable & component_variable) const;
};
