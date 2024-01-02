#pragma once

#include "WritableVectorAuxKernel.h"

/**
 * Construct a vector variable from 3 standard variables.
 */
class VectorVariableRandomizer : public WritableVectorAuxKernel
{
public:
  static InputParameters validParams();

  VectorVariableRandomizer(const InputParameters & parameters);

protected:
  virtual void compute() override;
};
