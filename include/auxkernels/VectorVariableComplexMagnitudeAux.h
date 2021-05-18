#pragma once

#include "AuxKernel.h"

/**
 * Compute the magnitude of a vector variable.
 * Deprecated in favour of ComplexMagnitude.
 */
class VectorVariableComplexMagnitudeAux : public AuxKernel
{
public:
  static InputParameters validParams();

  /**
   * Class constructor
   * @param parameters Input parameters for the object
   */
  VectorVariableComplexMagnitudeAux(const InputParameters & parameters);

protected:
  virtual Real computeValue() override;

private:
  /// Reference to vector variable value
  const VectorVariableValue & _real_vector;
  const VectorVariableValue & _imag_vector;

};
