#pragma once

#include "MFEMFormulation.h"
#include "factory.hpp"

class EBFormulation : public MFEMFormulation
{
public:
  static InputParameters validParams();

  EBFormulation(const InputParameters & parameters);
  virtual ~EBFormulation();

  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}
  virtual hephaestus::ProblemBuilder * getProblemBuilder() override { return &formulation; };

private:
  std::string e_field_name;
  std::string b_field_name;
  std::string magnetic_permeability_name;
  std::string electric_conductivity_name;
  hephaestus::EBDualFormulation formulation;
};
