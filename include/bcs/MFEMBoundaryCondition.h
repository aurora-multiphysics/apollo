#pragma once

#include "GeneralUserObject.h"
#include "boundary_conditions.hpp"
#include "gridfunctions.hpp"
#include "coefficients.hpp"
#include "Function.h"

class MFEMBoundaryCondition : public GeneralUserObject
{
public:
  static InputParameters validParams();

  MFEMBoundaryCondition(const InputParameters & parameters);
  virtual ~MFEMBoundaryCondition();

  virtual hephaestus::BoundaryCondition * getBC();
  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}
  virtual void storeCoefficients(hephaestus::Coefficients & coefficients) {}

protected:
  std::vector<BoundaryName> _boundary_names;
  mfem::Array<int> bdr_attr;
  hephaestus::BoundaryCondition * _boundary_condition;
};
