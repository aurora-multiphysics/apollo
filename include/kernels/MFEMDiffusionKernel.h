#pragma once
#include "MFEMBilinearFormKernel.h"
#include "kernels.hpp"

class MFEMDiffusionKernel : public MFEMBilinearFormKernel
{
public:
  static InputParameters validParams();

  MFEMDiffusionKernel(const InputParameters & parameters);
  virtual ~MFEMDiffusionKernel();

  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}

  virtual hephaestus::Kernel<mfem::ParBilinearForm> * getKernel() override;

protected:
  hephaestus::InputParameters kernel_params;
  hephaestus::DiffusionKernel kernel;
};
