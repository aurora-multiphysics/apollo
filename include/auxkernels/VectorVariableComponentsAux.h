#pragma once

#include "AuxKernel.h"

/**
 * Extract the components from a vector variable.
 */
class VectorVariableComponentsAux : public AuxKernel
{
public:
  static InputParameters validParams();

  VectorVariableComponentsAux(const InputParameters & parameters);

protected:
  virtual void compute() override final;

  // NB: not used.
  virtual Real computeValue() override final { mooseError("Unused"); }
};
