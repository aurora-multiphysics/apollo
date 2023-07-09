#pragma once

#include "MFEMBoundaryCondition.h"
#include "boundary_conditions.hpp"

libMesh::Point PointFromMFEMVector(const mfem::Vector & vec);

class MFEMComplexVectorFunctionDirichletBC : public MFEMBoundaryCondition
{
public:
  static InputParameters validParams();

  MFEMComplexVectorFunctionDirichletBC(const InputParameters & parameters);
  virtual ~MFEMComplexVectorFunctionDirichletBC();

  virtual hephaestus::BoundaryCondition * getBC() override;
  virtual void storeCoefficients(hephaestus::Coefficients & coefficients) override;
  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}

protected:
  const Function & _func_re;
  const Function & _func_im;
  mfem::VectorFunctionCoefficient _vec_function_coef_re;
  mfem::VectorFunctionCoefficient _vec_function_coef_im;
};
