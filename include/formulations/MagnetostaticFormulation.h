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
  virtual hephaestus::ProblemBuilder * getProblemBuilder() override { return &formulation; };

private:
  std::string magnetic_vector_potential_name;
  std::string magnetic_permeability_name;
  std::string magnetic_reluctivity_name;
  hephaestus::MagnetostaticFormulation formulation;
};
