#pragma once

#include "MFEMBoundaryCondition.h"
#include "MFEMVectorFunctionCoefficient.h"
#include "boundary_conditions.hpp"

class MFEMComplexVectorDirichletBC : public MFEMBoundaryCondition
{
public:
  static InputParameters validParams();

  MFEMComplexVectorDirichletBC(const InputParameters & parameters);
  virtual ~MFEMComplexVectorDirichletBC();

  virtual hephaestus::BoundaryCondition * getBC() override;
  virtual void storeCoefficients(hephaestus::Coefficients & coefficients) override;
  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}

protected:
  MFEMVectorCoefficient * _vec_coef_re;
  MFEMVectorCoefficient * _vec_coef_im;
  hephaestus::VectorDirichletBC _boundary_condition;
};
