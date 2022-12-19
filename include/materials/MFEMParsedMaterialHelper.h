//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "FunctionParserUtils.h"
#include "auxkernels.hpp"

#include "libmesh/fparser_ad.hh"

#define usingMFEMParsedMaterialHelperMembers()                                                     \
  usingFunctionParserUtilsMembers();                                                               \
  using typename MFEMParsedMaterialHelper::VariableNameMappingMode;                                \
  using typename MFEMParsedMaterialHelper::MatPropDescriptorList;                                  \
  using MFEMParsedMaterialHelper::functionParse;                                                   \
  using MFEMParsedMaterialHelper::functionsPostParse;                                              \
  using MFEMParsedMaterialHelper::functionsOptimize;                                               \
  using MFEMParsedMaterialHelper::_func_F;                                                         \
  using MFEMParsedMaterialHelper::_symbol_names;                                                   \
  using MFEMParsedMaterialHelper::_gridfunction_names;                                             \
  using MFEMParsedMaterialHelper::_tol;                                                            \
  using MFEMParsedMaterialHelper::_coefficient_names;                                              \
  using MFEMParsedMaterialHelper::_map_mode

/**
 * Helper class to perform the parsing and optimization of the
 * function expression.
 */
class MFEMParsedMaterialHelper : public MFEMMaterial,
                                 public hephaestus::CoupledCoefficient,
                                 public FunctionParserUtils<false>
{
public:
  enum class VariableNameMappingMode
  {
    USE_MOOSE_NAMES,
    USE_PARAM_NAMES
  };

  MFEMParsedMaterialHelper(const InputParameters & parameters, VariableNameMappingMode map_mode);

  static InputParameters validParams();

  void functionParse(const std::string & function_expression);
  void functionParse(const std::string & function_expression,
                     const std::vector<std::string> & constant_names,
                     const std::vector<std::string> & constant_expressions);
  void functionParse(const std::string & function_expression,
                     const std::vector<std::string> & constant_names,
                     const std::vector<std::string> & constant_expressions,
                     const std::vector<std::string> & _mfem_coefficient_names,
                     const std::vector<std::string> & tol_names,
                     const std::vector<Real> & tol_values);
  void functionParse(const std::string & function_expression,
                     const std::vector<std::string> & constant_names,
                     const std::vector<std::string> & constant_expressions,
                     const std::vector<std::string> & _mfem_coefficient_names,
                     const std::vector<std::string> & _mfem_gridfunction_names,
                     const std::vector<std::string> & tol_names,
                     const std::vector<Real> & tol_values);
  void Init(const mfem::NamedFieldsMap<mfem::ParGridFunction> & variables,
            hephaestus::DomainProperties & domain_properties) override;

  double Eval(mfem::ElementTransformation & trans, const mfem::IntegrationPoint & ip) override;

protected:
  usingFunctionParserUtilsMembers(false);

  void computeQpProperties();

  // run FPOptimizer on the parsed function
  virtual void functionsOptimize();

  /// The undiffed free energy function parser object.
  SymFunctionPtr _func_F;

  /**
   * Symbol names used in the expression (depends on the map_mode).
   * We distinguish "symbols" i.e. FParser placeholder names from "variables", which
   * are MOOSE solution objects
   */
  std::vector<std::string> _symbol_names;

  std::vector<std::string> _gridfunction_names;
  std::vector<mfem::ParGridFunction *> _gridfunctions;

  std::vector<std::string> _coefficient_names;
  std::vector<mfem::Coefficient *> _coefficients;

  /// Tolerance values for all arguments (to protect from log(0)).
  std::vector<Real> _tol;

  /**
   * Flag to indicate if MOOSE nonlinear variable names should be used as FParser variable names.
   * This should be USE_MOOSE_NAMES only for DerivativeParsedMaterial. If set to USE_PARAM_NAMES,
   * this class looks up the input parameter name for each coupled variable and uses it as the
   * FParser parameter name when parsing the FParser expression.
   */
  const VariableNameMappingMode _map_mode;

  /// This is true by default, but can be disabled to make non-existing properties default to zero
  const bool _error_on_missing_material_properties;
};
