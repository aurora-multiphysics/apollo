#pragma once

#include "MFEMBoundaryCondition.h"
#include "boundary_conditions.hpp"

libMesh::Point PointFromMFEMVector(const mfem::Vector & vec);

class MFEMVectorFunctionDirichletBC : public MFEMBoundaryCondition
{
public:
  static InputParameters validParams();

  MFEMVectorFunctionDirichletBC(const InputParameters & parameters);
  virtual ~MFEMVectorFunctionDirichletBC();

  virtual hephaestus::BoundaryCondition * getBC() override;
  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}

protected:
  const Function & _func;
  mfem::FunctionCoefficient _function_coeff;
};
