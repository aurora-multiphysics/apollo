#pragma once

#include "MFEMCoefficient.h"
#include "gridfunctions.hpp"

class MFEMConstantCoefficient : public MFEMCoefficient
{
public:
  static InputParameters validParams();

  MFEMConstantCoefficient(const InputParameters & parameters);
  virtual ~MFEMConstantCoefficient();

  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}
  virtual mfem::Coefficient * getCoefficient() override { return &coefficient; };

private:
  mfem::ConstantCoefficient coefficient;
};
