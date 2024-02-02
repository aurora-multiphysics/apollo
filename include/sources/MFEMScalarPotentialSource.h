#pragma once
#include "MFEMSource.h"

class MFEMScalarPotentialSource : public MFEMSource
{
public:
  static InputParameters validParams();

  MFEMScalarPotentialSource(const InputParameters & parameters);
  virtual ~MFEMScalarPotentialSource() override {}

  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}

protected:
  const MFEMVariable & _source_grad_potential_gridfunction;
  const MFEMVariable & _source_potential_gridfunction;
  std::string _conductivity_coef_name;
  const MFEMFESpace & _hcurl_fespace;
  const MFEMFESpace & _h1_fespace;
};
