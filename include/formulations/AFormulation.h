#pragma once

#include "MFEMFormulation.h"
#include "factory.hpp"

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
