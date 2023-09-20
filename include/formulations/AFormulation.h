#pragma once

#include "MFEMFormulation.h"
#include "factory.hpp"
/*
Solves:MFEM formulation for solving:
∇×(ν∇×A) + σdA/dt = Jᵉ

in weak form
(ν∇×A, ∇×A') + (σdA/dt, A') - (Jᵉ, A') - <(ν∇×A)×n, A'>  = 0

where:
Magnetic reluctivity ν = 1/μ
Electrical conductivity σ=1/ρ
Magnetic vector potential A
Electric field, E = ρJᵉ -dA/dt
Magnetic flux density, B = ∇×A
Magnetic field H = ν∇×A
Current density J = Jᵉ -σdA/dt
*/
class AFormulation : public MFEMFormulation
{
public:
  static InputParameters validParams();

  AFormulation(const InputParameters & parameters);
  virtual ~AFormulation();

  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}
  virtual hephaestus::ProblemBuilder * getProblemBuilder() override { return &formulation; };

private:
  std::string magnetic_vector_potential_name;
  std::string magnetic_permeability_name;
  std::string electric_conductivity_name;
  std::string magnetic_reluctivity_name;
  hephaestus::AFormulation formulation;
};
