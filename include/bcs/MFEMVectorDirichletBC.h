#pragma once

#include "MFEMBoundaryCondition.h"
#include "MFEMVectorFunctionCoefficient.h"
#include "boundary_conditions.hpp"

class MFEMVectorDirichletBC : public MFEMBoundaryCondition
{
public:
  static InputParameters validParams();

  MFEMVectorDirichletBC(const InputParameters & parameters);
  virtual ~MFEMVectorDirichletBC();

  virtual hephaestus::BoundaryCondition * getBC() override;
  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}

protected:
  MFEMVectorCoefficient * _vec_coef;
  hephaestus::VectorDirichletBC _boundary_condition;
};
