#pragma once

#include "ElementIntegralPostprocessor.h"

class Function;

// Forward Declarations
class ElementNedelecL2Error;

template <>
InputParameters validParams<ElementNedelecL2Error>();

class ElementNedelecL2Error : public ElementIntegralPostprocessor
{
public:
  static InputParameters validParams();

  ElementNedelecL2Error(const InputParameters & parameters);

  virtual Real getValue() override;

protected:
  virtual Real computeQpIntegral() override;

  const Function & _function;

  const VectorVariableValue & _u; // FE vector solution
};
