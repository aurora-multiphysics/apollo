#pragma once

#include "MFEMCoefficient.h"
#include "gridfunctions.hpp"

class MFEMFunctionCoefficient : public MFEMCoefficient
{
public:
  static InputParameters validParams();

  MFEMFunctionCoefficient(const InputParameters & parameters);
  virtual ~MFEMFunctionCoefficient();

  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}
  virtual mfem::Coefficient * getCoefficient() override { return &_coefficient; };

private:
  const Function & _func;
  mfem::FunctionCoefficient _coefficient;
};
