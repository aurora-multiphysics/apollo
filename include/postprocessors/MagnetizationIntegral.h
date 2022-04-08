#pragma once

#include "ElementIntegralPostprocessor.h"

/**
 * This postprocessor computes a volume integral of the specified variable.
 *
 * Note that specializations of this integral are possible by deriving from this
 * class and overriding computeQpIntegral().
 */
class MagnetizationIntegral : public ElementIntegralPostprocessor
{
public:
  static InputParameters validParams();

  MagnetizationIntegral(const InputParameters & parameters);

protected:
  virtual Real computeQpIntegral() override;

  /// Will hold 0, 1, or 2 corresponding to x, y, or z.
  int _component;
  /// Holds the solution at current quadrature points
  const VectorVariableValue & _u;
  /// Holds the solution curl at the current quadrature points
  const VectorVariableCurl & _curl_u;
};
