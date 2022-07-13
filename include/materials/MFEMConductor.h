#pragma once

#include "MFEMMaterial.h"
#include "materials.hpp"

class MFEMConductor : public MFEMMaterial
{
public:
  static InputParameters validParams();

  MFEMConductor(const InputParameters & parameters);
  virtual ~MFEMConductor();

  /// EM constants
  static const Real _mu0;
  static const Real _epsilon0;

protected:
  const Real & _input_electrical_conductivity;
  const Real & _input_electric_permittivity;
  const Real & _input_magnetic_permeability;
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
