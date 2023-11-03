#pragma once
#include "MFEMVectorCoefficient.h"

class MFEMVectorFunctionCoefficient : public MFEMVectorCoefficient
{
public:
  static InputParameters validParams();

  MFEMVectorFunctionCoefficient(const InputParameters & parameters);
  virtual ~MFEMVectorFunctionCoefficient();

  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}
  virtual mfem::VectorFunctionCoefficient * getVectorCoefficient() override
  {
    return &_vector_coefficient;
  };

private:
  const Function & _func;
  mfem::VectorFunctionCoefficient _vector_coefficient;
};
