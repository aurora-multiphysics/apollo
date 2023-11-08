#pragma once

#include "VectorVariableFromComponentsAux.h"

/**
 * Set 3 standard variables from a vector variable.
 */
class VectorVariableComponentsAux : public VectorVariableFromComponentsAux
{
public:
  static InputParameters validParams();

  VectorVariableComponentsAux(const InputParameters & parameters);

protected:
  virtual void compute() override;
};
