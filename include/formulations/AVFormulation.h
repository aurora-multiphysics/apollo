#pragma once

#include "MFEMFormulation.h"
#include "factory.hpp"
/*
MFEM formulation for solving:
∇×(ν∇×A) + σ(dA/dt + ∇ V) = Jᵉ
∇·(σ(dA/dt + ∇ V))= 0

where
Jᵉ ∈ H(div) source field
A ∈ H(curl)
V ∈ H1

via the weak form:
(ν∇×A, ∇×A') + (σ(dA/dt + ∇ V), A') - (Jᵉ, A') - <(ν∇×A) × n, A'>  = 0
(σ(dA/dt + ∇ V), ∇V') - <σ(dA/dt + ∇ V)·n, V'> =0

where:
Magnetic reluctivity ν = 1/μ
Electrical conductivity σ=1/ρ
Magnetic vector potential A
Scalar electric potential V
Electric field, E = -dA/dt -∇V
Magnetic flux density, B = ∇×A
Magnetic field H = ν∇×A
Current density J = -σ(dA/dt + ∇ V)

Either:
B.n (or E×n) at boundary: A×n (Dirichlet)
H×n at boundary: ν∇×A (Integrated)
-σ(dA/dt + ∇ V)·n (J·n, Neumann), V (potential, Dirichlet)
*/
class AVFormulation : public MFEMFormulation
{
public:
  static InputParameters validParams();

  AVFormulation(const InputParameters & parameters);
  virtual ~AVFormulation();

  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}

  std::shared_ptr<hephaestus::ProblemBuilder> getProblemBuilder() override { return formulation; }

private:
  std::string magnetic_vector_potential_name;
  std::string electric_potential_name;
  std::string magnetic_permeability_name;
  std::string electric_conductivity_name;
  std::string magnetic_reluctivity_name;
  std::shared_ptr<hephaestus::AVFormulation> formulation{nullptr};
};
