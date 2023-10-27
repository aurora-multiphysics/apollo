#pragma once

#include "AuxKernel.h"

class BuildVectorVariableAux : public VectorAuxKernel
{
public:
  static InputParameters validParams();

  BuildVectorVariableAux(const InputParameters & parameters);

protected:
  virtual void compute() override final;

  // Not used.
  virtual RealVectorValue computeValue() override final { mooseError("Unused"); }

  const std::vector<const VariableValue *> _component_dofs;
};
