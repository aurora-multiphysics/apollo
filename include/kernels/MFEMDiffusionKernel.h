#pragma once
#include "MFEMBilinearFormKernel.h"
#include "kernels.hpp"

class MFEMDiffusionKernel : public MFEMBilinearFormKernel
{
public:
  static InputParameters validParams();

  MFEMDiffusionKernel(const InputParameters & parameters);
  ~MFEMDiffusionKernel() override {}

  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}

  // NB: unique pointer releases ownership when called. Caller is responsible for deleting kernel.
  hephaestus::Kernel<mfem::ParBilinearForm> * getKernel() override;

protected:
  hephaestus::InputParameters _kernel_params;
  std::unique_ptr<hephaestus::DiffusionKernel> _kernel{nullptr};
};
