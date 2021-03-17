//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "ElementIntegralPostprocessor.h"

// Forward Declarations
class MagnetizationIntegral;

template <>
InputParameters validParams<MagnetizationIntegral>();

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
