//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "MFEMParsedCoefficientHelper.h"
#include "MFEMParsedCoefficientBase.h"

/**
 * MFEMMAterial child class to evaluate a parsed function. The function
 * can access non-linear and aux variables (unlike MooseParsedFunction).
 */
class MFEMParsedCoefficient : public MFEMParsedCoefficientHelper, public MFEMParsedCoefficientBase
{
public:
  static InputParameters validParams();

  MFEMParsedCoefficient(const InputParameters & parameters);
};
