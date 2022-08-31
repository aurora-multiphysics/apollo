#pragma once

#include "GeneralUserObject.h"
#include "Function.h"
#include "variables.hpp"

class MFEMVariable : public GeneralUserObject
{
public:
  static InputParameters validParams();

  MFEMVariable(const InputParameters & parameters);
  virtual ~MFEMVariable();
  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}

  hephaestus::InputParameters mfem_params;

protected:
  std::string fespace_name;
  std::string fespace_type;
  int order;
  int components;
};
