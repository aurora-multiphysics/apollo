#pragma once

#include "MFEMCoefficient.h"
#include "materials.hpp"

class MFEMConstantCoefficient : public MFEMCoefficient, public mfem::ConstantCoefficient
{
public:
  static InputParameters validParams();

  MFEMConstantCoefficient(const InputParameters & parameters);
  virtual ~MFEMConstantCoefficient();

  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}
};
