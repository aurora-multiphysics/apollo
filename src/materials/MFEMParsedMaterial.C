//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "MFEMParsedMaterial.h"

registerMooseObject("MooseApp", MFEMParsedMaterial);

InputParameters
MFEMParsedMaterial::validParams()
{
  InputParameters params = MFEMParsedMaterialHelper::validParams();
  params += MFEMParsedMaterialBase::validParams();
  params.addClassDescription("MFEM Parsed Function Material.");
  return params;
}

MFEMParsedMaterial::MFEMParsedMaterial(const InputParameters & parameters)
  : MFEMParsedMaterialHelper(parameters, VariableNameMappingMode::USE_MOOSE_NAMES),
    MFEMParsedMaterialBase(parameters)
{
  // Build function and optimize
  functionParse(_function,
                _constant_names,
                _constant_expressions,
                getParam<std::vector<std::string>>("mfem_coefficient_names"),
                getParam<std::vector<std::string>>("mfem_gridfunction_names"),
                _tol_names,
                _tol_values);
}
