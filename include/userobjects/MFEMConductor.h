#pragma once

#include "GeneralUserObject.h"
#include "materials.hpp"

class MFEMConductor : public GeneralUserObject
{
public:
  static InputParameters validParams();

  MFEMConductor(const InputParameters & parameters);
  virtual ~MFEMConductor();

  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}

  /// EM constants
  static const Real _mu0;
  static const Real _epsilon0;

  std::map<std::string, mfem::Coefficient *> scalar_property_map;

protected:

  const Real & _input_electrical_conductivity;
  const Real & _input_rel_permittivity;
  const Real & _input_rel_permeability;
  const Real & _input_thermal_conductivity;
  const Real & _input_heat_capacity;

  mfem::ConstantCoefficient _electrical_conductivity_coeff;
  mfem::ConstantCoefficient _permittivity_coeff;
  mfem::ConstantCoefficient _permeability_coeff;
  mfem::ConstantCoefficient _resistivity_coeff;
  mfem::ConstantCoefficient _reluctivity_coeff;

  mfem::ConstantCoefficient _heat_capacity_coeff;
  mfem::ConstantCoefficient _inverse_heat_capacity_coeff;
  mfem::ConstantCoefficient _thermal_conductivity_coeff;
  mfem::ConstantCoefficient _inverse_thermal_conductivity_coeff;
};
