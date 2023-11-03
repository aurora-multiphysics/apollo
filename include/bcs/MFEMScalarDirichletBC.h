#pragma once

#include "MFEMBoundaryCondition.h"
#include "MFEMFunctionCoefficient.h"
#include "boundary_conditions.hpp"

class MFEMScalarDirichletBC : public MFEMBoundaryCondition
{
public:
  static InputParameters validParams();

  MFEMScalarDirichletBC(const InputParameters & parameters);

  virtual hephaestus::BoundaryCondition * getBC() override;
  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}

protected:
  MFEMCoefficient * _coef;
  hephaestus::ScalarDirichletBC _boundary_condition;
};
