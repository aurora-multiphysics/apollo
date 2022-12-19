//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "InputParameters.h"

/**
 * Helper class for MFEMParsedCoefficient
 * to declare and read the input parameters.
 */
class MFEMParsedCoefficientBase
{
public:
  static InputParameters validParams();

  MFEMParsedCoefficientBase(const InputParameters & parameters);

protected:
  /// function expression
  std::string _function;

  /// constant vectors
  std::vector<std::string> _constant_names;
  std::vector<std::string> _constant_expressions;

  /// constant vectors
  std::vector<std::string> _mfem_coefficient_names;
  std::vector<std::string> _mfem_gridfunction_names;

  /// tolerance vectors
  std::vector<std::string> _tol_names;
  std::vector<Real> _tol_values;
};
