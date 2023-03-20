#pragma once

#include "MFEMBoundaryCondition.h"
#include "boundary_conditions.hpp"

class MFEMFunctionDirichletBC : public MFEMBoundaryCondition
{
public:
  static InputParameters validParams();

  MFEMFunctionDirichletBC(const InputParameters & parameters);

  virtual hephaestus::BoundaryCondition * getBC() override;
  virtual void storeCoefficients(hephaestus::DomainProperties & domain_properties) override;
  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}

protected:
  const Function & _func;
  mfem::FunctionCoefficient _function_coeff;
};
