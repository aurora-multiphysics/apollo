#pragma once
#include "MFEMSource.h"

class MFEMScalarPotentialSource : public MFEMSource
{
public:
  static InputParameters validParams();

  MFEMScalarPotentialSource(const InputParameters & parameters);
  virtual ~MFEMScalarPotentialSource();

  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}

  virtual hephaestus::Source * getSource() override;
  virtual void storeCoefficients(hephaestus::Coefficients & coefficients) override;

protected:
  std::string source_coef_name;
  std::string potential_name;
  std::string grad_electric_potential_name;
  std::string conductivity_coef_name;
  const MFEMFESpace & hcurl_fespace;
  const MFEMFESpace & h1_fespace;

  mfem::PWVectorCoefficient * _restricted_coef;
};
