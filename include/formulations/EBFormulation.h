#pragma once

#include "MFEMFormulation.h"
#include "factory.hpp"

/*
MFEM formulation for solving the equations
∇⋅Jᵉ = 0
∇×(νB) - σE = Jᵉ
dB/dt = -∇×E

where
Jᵉ ∈ H(div) source field
B ∈ H(div)
E ∈ H(curl)
p ∈ H1

Weak form (Space discretisation)
-(Jᵉ, ∇ p') + <n.Jᵉ, p'> = 0
(νB, ∇×E') - (σE, E') - (Jᵉ, E') - <(νB) × n, E'> = 0
(dB/dt, B') + (∇×E, B') = 0

Time discretisation using implicit scheme:
Unknowns
Jᵉ_{n+1} ∈ H(div) source field, where Jᵉ = -σ∇p
dB/dt_{n+1} ∈ H(div)
E_{n+1} ∈ H(curl)
p_{n+1} ∈ H1

Fully discretised equations
-(Jᵉ_{n+1}, ∇ p') + <n.Jᵉ_{n+1}, p'> = 0
(νB_{n}, ∇×E') - (νdt∇×E_{n+1}, ∇×E') - (σE_{n+1}, E') - (Jᵉ_{n+1}, E') -
<(νB) × n, E'> = 0
(dB/dt_{n+1}, B') + (∇×E_{n+1}, B') = 0
using
B_{n+1} = B_{n} + dt dB/dt_{n+1} = B_{n} - dt ∇×E_{n+1}
*/
class EBFormulation : public MFEMFormulation
{
public:
  static InputParameters validParams();

  EBFormulation(const InputParameters & parameters);
  virtual ~EBFormulation();

  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}

  std::shared_ptr<hephaestus::ProblemBuilder> getProblemBuilder() override { return formulation; }

private:
  std::string e_field_name;
  std::string b_field_name;
  std::string magnetic_permeability_name;
  std::string electric_conductivity_name;
  std::string magnetic_reluctivity_name;
  std::shared_ptr<hephaestus::EBDualFormulation> formulation{nullptr};
};
