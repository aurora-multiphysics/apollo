#pragma once

#include "MFEMFormulation.h"
#include "factory.hpp"

class ComplexEFormulation : public MFEMFormulation
{
public:
  static InputParameters validParams();

  ComplexEFormulation(const InputParameters & parameters);
  virtual ~ComplexEFormulation();

  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}
  virtual hephaestus::ProblemBuilder * getProblemBuilder() override { return &formulation; };

private:
  std::string e_field_re_name;
  std::string e_field_im_name;
  std::string magnetic_permeability_name;
  std::string electric_conductivity_name;
  std::string dielectric_permittivity_name;
  hephaestus::ComplexEFormulation formulation;
};
