#pragma once

#include "MFEMBoundaryCondition.h"
#include "boundary_conditions.hpp"

libMesh::Point PointFromMFEMVector(const mfem::Vector & vec);

class MFEMVectorNormalIntegratedBC : public MFEMBoundaryCondition
{
public:
  static InputParameters validParams();

  MFEMVectorNormalIntegratedBC(const InputParameters & parameters);
  virtual ~MFEMVectorNormalIntegratedBC();

  virtual hephaestus::BoundaryCondition * getBC() override;
  virtual void storeCoefficients(hephaestus::Coefficients & coefficients) override;
  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}

protected:
  const Function & _func;
  mfem::VectorFunctionCoefficient _vec_function_coef;
};
