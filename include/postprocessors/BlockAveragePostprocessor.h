//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "GeneralPostprocessor.h"
#include "BlockAverageValue.h"
// Forward Declarations
class BlockAveragePostprocessor;

template <>
InputParameters validParams<BlockAveragePostprocessor>();

/**
 *
 */
class BlockAveragePostprocessor : public GeneralPostprocessor
{
public:
  static InputParameters validParams();

  BlockAveragePostprocessor(const InputParameters & parameters);

  virtual void initialize() override;
  virtual void execute() override;
  virtual Real getValue() override;

protected:
  unsigned int _block;
  const BlockAverageValue & _block_average_value;
};
