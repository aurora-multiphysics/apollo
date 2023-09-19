#pragma once

#include "MFEMFormulation.h"
#include "factory.hpp"

/*
MFEM formulation for solving:
∇×(ν∇×A) + iωσA - ω²εA = J

via the weak form:
(ν∇×A, ∇×A') + (iωσA, A') - (ω²εA, A') - <(ν∇×A)×n, A'> = (J, A')

where
A ∈ H(curl) is the complex magnetic vector potential
J ∈ H(div) is the divergence-free source current density
ω is the angular frequency
ν is the reluctivity (reciprocal of magnetic permeability; 1/µ)
σ is the electric conductivity
ε is the electric permittivity

Dirichlet boundaries strongly constrain n×n×A
Integrated boundaries weakly constrain (ν∇×A)×n = H×n
Robin boundaries weakly constrain (ν∇×A)×n + γ(n×n×A) = F

Divergence cleaning (such as via Helmholtz projection)
should be performed on J before use in this operator.
*/
class ComplexAFormulation : public MFEMFormulation
{
public:
  static InputParameters validParams();

  ComplexAFormulation(const InputParameters & parameters);
  virtual ~ComplexAFormulation();

  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}
  virtual hephaestus::ProblemBuilder * getProblemBuilder() override { return &formulation; };

private:
  std::string magnetic_vector_potential_name;
  std::string magnetic_vector_potential_re_name;
  std::string magnetic_vector_potential_im_name;
  std::string frequency_name;
  std::string magnetic_permeability_name;
  std::string electric_conductivity_name;
  std::string dielectric_permittivity_name;
  std::string magnetic_reluctivity_name;
  hephaestus::ComplexAFormulation formulation;
};
