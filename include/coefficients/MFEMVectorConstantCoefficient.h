#pragma once
#include "MFEMVectorCoefficient.h"

class MFEMVectorConstantCoefficient : public MFEMVectorCoefficient
{
public:
  static InputParameters validParams();

  MFEMVectorConstantCoefficient(const InputParameters & parameters);
  virtual ~MFEMVectorConstantCoefficient();

  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}
  virtual mfem::VectorCoefficient * getVectorCoefficient() override
  {
    return &_vector_coefficient;
  };

private:
  const mfem::Vector _vector;
  mfem::VectorConstantCoefficient _vector_coefficient;
};
