#pragma once

#include "MFEMFormulation.h"
#include "factory.hpp"

/*
MFEM formulation for solving:
∇×(ν∇×E) + σdE/dt = -dJᵉ/dt

via the weak form
(ν∇×E, ∇×E') + (σdE/dt, E') + (dJᵉ/dt, E') - <(ν∇×E)×n, E'>  = 0

where:
Magnetic reluctivity ν = 1/μ
Electrical conductivity σ=1/ρ
Electric Field E
Current density J = σE
Magnetic flux density, dB/dt = -∇×E
Magnetic field dH/dt = -ν∇×E
*/
class EFormulation : public MFEMFormulation
{
public:
  static InputParameters validParams();

  EFormulation(const InputParameters & parameters);
  virtual ~EFormulation();

  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}

  std::shared_ptr<hephaestus::ProblemBuilder> getProblemBuilder() override { return formulation; };

private:
  std::string _electric_field_name;
  std::string _magnetic_permeability_name;
  std::string _electric_conductivity_name;
  std::string _magnetic_reluctivity_name;

  std::string _current_density_name;
  std::string _joule_heating_density_name;
  std::string _external_current_density_name;

  std::shared_ptr<hephaestus::EFormulation> formulation{nullptr};
};
