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
