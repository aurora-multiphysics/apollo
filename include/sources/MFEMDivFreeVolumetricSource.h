#pragma once
#include "MFEMSource.h"
#include "MFEMVectorFunctionCoefficient.h"

class MFEMDivFreeVolumetricSource : public MFEMSource
{
public:
  static InputParameters validParams();

  MFEMDivFreeVolumetricSource(const InputParameters & parameters);
  virtual ~MFEMDivFreeVolumetricSource();

  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}

  virtual void storeCoefficients(hephaestus::Coefficients & coefficients) override;

protected:
  MFEMVectorCoefficient * _vec_coef;
  mfem::Array<mfem::VectorCoefficient *> sourcecoefs;
  mfem::Array<int> coilsegments;
  std::string source_coef_name;
  const MFEMFESpace & hcurl_fespace;
  const MFEMFESpace & h1_fespace;

  mfem::PWVectorCoefficient * _restricted_coef;
};
