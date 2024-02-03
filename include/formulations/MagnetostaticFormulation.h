#pragma once

#include "MFEMFormulation.h"
#include "factory.hpp"
/*
Solves:MFEM formulation for solving:
∇×(ν∇×A) = Jᵉ

in weak form
(ν∇×A, ∇×A') - (Jᵉ, A') - <(ν∇×A)×n, A'>  = 0

where:
Magnetic reluctivity ν = 1/μ
Magnetic vector potential A
Magnetic flux density, B = ∇×A
Magnetic field H = ν∇×A
*/
class MagnetostaticFormulation : public MFEMFormulation
{
public:
  static InputParameters validParams();

  MagnetostaticFormulation(const InputParameters & parameters);
  virtual ~MagnetostaticFormulation();

  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}

  std::shared_ptr<hephaestus::ProblemBuilder> getProblemBuilder() override { return formulation; }

private:
  std::string _magnetic_vector_potential_name;
  std::string _magnetic_permeability_name;
  std::string _magnetic_reluctivity_name;

  std::string _magnetic_field_name;
  std::string _magnetic_flux_density_name;
  std::string _lorentz_force_density_name;

  std::string _external_magnetic_field_name;
  std::string _external_current_density_name;
  std::string _external_magnetic_flux_density_name;
  std::shared_ptr<hephaestus::MagnetostaticFormulation> formulation{nullptr};
};
