#pragma once

#include "MFEMMaterial.h"
#include "auxkernels.hpp"
#include "materials.hpp"

class TDependentConductivityCoefficient : public hephaestus::CoupledCoefficient
{
public:
  TDependentConductivityCoefficient(const hephaestus::InputParameters & params)
    : hephaestus::CoupledCoefficient(params){};
  virtual double Eval(mfem::ElementTransformation & trans, const mfem::IntegrationPoint & ip)
  {
    double alpha0 = 4.29e-6; // Resistivity temperature coefficient
    double sigma0 = 5.96e7;  // Conductivity at T= 273 K
    double T = gf->GetValue(trans, ip);
    double T0 = 273; // Reference temperature
    return sigma0 / (1 + alpha0 * (T - T0));
  }
};

class MFEMTemperatureDependentConductor : public MFEMMaterial
{
public:
  static InputParameters validParams();

  MFEMTemperatureDependentConductor(const InputParameters & parameters);
  virtual ~MFEMTemperatureDependentConductor();

  /// EM constants
  static const Real _mu0;
  static const Real _epsilon0;

protected:
  const Real & _input_electrical_conductivity;
  const Real & _input_electric_permittivity;
  const Real & _input_magnetic_permeability;
  const Real & _input_thermal_conductivity;
  const Real & _input_heat_capacity;

  hephaestus::InputParameters _elec_cond_coef_inputs;
  TDependentConductivityCoefficient _electrical_conductivity_coeff;

  mfem::ConstantCoefficient _permittivity_coeff;
  mfem::ConstantCoefficient _permeability_coeff;
  mfem::ConstantCoefficient _resistivity_coeff;
  mfem::ConstantCoefficient _reluctivity_coeff;

  mfem::ConstantCoefficient _heat_capacity_coeff;
  mfem::ConstantCoefficient _inverse_heat_capacity_coeff;
  mfem::ConstantCoefficient _thermal_conductivity_coeff;
  mfem::ConstantCoefficient _inverse_thermal_conductivity_coeff;
};
