#pragma once

#include "MFEMBoundaryCondition.h"
#include "MFEMFunctionCoefficient.h"
#include "boundary_conditions.hpp"

class MFEMFunctionDirichletBC : public MFEMBoundaryCondition
{
public:
  static InputParameters validParams();

  MFEMFunctionDirichletBC(const InputParameters & parameters);

  virtual hephaestus::BoundaryCondition * getBC() override;
  virtual void storeCoefficients(hephaestus::Coefficients & coefficients) override;
  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}

protected:
  MFEMFunctionCoefficient * _function_coef;
  hephaestus::FunctionDirichletBC _boundary_condition;
};
