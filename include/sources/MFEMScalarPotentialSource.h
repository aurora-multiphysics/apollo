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
  std::string source_coef_name;
  std::string potential_name;
  std::string grad_potential_name;
  std::string conductivity_coef_name;
  const MFEMFESpace & hcurl_fespace;
  const MFEMFESpace & h1_fespace;
};
