#pragma once

#include "IntegratedBC.h"

class FunctionFluxBC : public IntegratedBC
{
public:
  static InputParameters validParams();

  FunctionFluxBC(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();

  /// Optional vectorValue function
  const Function * const _function;

  /// x component function
  const Function & _function_x;
  /// y component function
  const Function & _function_y;
  /// z component function
  const Function & _function_z;

  /// The value for this BC
  RealVectorValue _values;
};
