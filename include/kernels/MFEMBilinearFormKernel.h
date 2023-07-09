#pragma once

#include "GeneralUserObject.h"
#include "kernels.hpp"
#include "gridfunctions.hpp"

class MFEMBilinearFormKernel : public GeneralUserObject
{
public:
  static InputParameters validParams();

  MFEMBilinearFormKernel(const InputParameters & parameters);
  virtual ~MFEMBilinearFormKernel();

  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}

  virtual hephaestus::Kernel<mfem::ParBilinearForm> * getKernel() { return NULL; };
};
