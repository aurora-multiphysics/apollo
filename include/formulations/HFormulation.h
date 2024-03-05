#pragma once

#include "MFEMFormulation.h"
#include "factory.hpp"

/*
MFEM formulation for solving:
∇×(ρ∇×H) + μdH/dt = -dBᵉ/dt

via the weak form
(ρ∇×H, ∇×v) + (μdH/dt, v) + (dBᵉ/dt, v) - <(ρ∇×H)×n, v>  = 0

where:
Magnetic reluctivity ν = 1/μ
Electrical conductivity σ=1/ρ
Magnetic field, E = ρ∇×H
Magnetic flux density, B = Bᵉ + μH
Current density J = ∇×H
*/
class HFormulation : public MFEMFormulation
{
public:
  static InputParameters validParams();

  HFormulation(const InputParameters & parameters);
  virtual ~HFormulation();

  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}

  std::shared_ptr<hephaestus::ProblemBuilder> getProblemBuilder() override { return formulation; };

private:
  std::string _magnetic_field_name;
  std::string _magnetic_permeability_name;
  std::string _electric_conductivity_name;
  std::string _electric_resistivity_name;

  std::string _electric_field_name;
  std::string _current_density_name;
  std::string _magnetic_flux_density_name;
  std::string _lorentz_force_density_name;
  std::string _joule_heating_density_name;

  std::string _external_electric_field_name;
  std::string _external_magnetic_field_name;
  std::string _external_current_density_name;
  std::string _external_magnetic_flux_density_name;

  std::shared_ptr<hephaestus::HFormulation> formulation{nullptr};
};
