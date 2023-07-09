#pragma once
#include "MFEMSource.h"

class MFEMDivFreeVolumetricSource : public MFEMSource
{
public:
  static InputParameters validParams();

  MFEMDivFreeVolumetricSource(const InputParameters & parameters);
  virtual ~MFEMDivFreeVolumetricSource();

  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}

  virtual hephaestus::Source * getSource() override;
  virtual void storeCoefficients(hephaestus::Coefficients & coefficients) override;

protected:
  const Function & _func;
  mfem::VectorFunctionCoefficient _vec_function_coef;
  mfem::Array<mfem::VectorCoefficient *> sourcecoefs;
  mfem::Array<int> coilsegments;
  std::string source_coef_name;

  mfem::PWVectorCoefficient * _restricted_coef;
};
