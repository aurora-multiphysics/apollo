#pragma once

#include "GeneralUserObject.h"
#include "factory.hpp"

class MFEMFormulation : public GeneralUserObject
{
public:
  static InputParameters validParams();

  MFEMFormulation(const InputParameters & parameters);
  virtual ~MFEMFormulation();

  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}
  std::string formulation_name;
  hephaestus::ProblemBuilder * problem_builder;
};
