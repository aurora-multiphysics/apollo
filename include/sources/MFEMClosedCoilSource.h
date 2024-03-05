#pragma once
#include "MFEMSource.h"
#include "MFEMVariable.h"
#include "MFEMCoefficient.h"

class MFEMClosedCoilSource : public MFEMSource
{
public:
  static InputParameters validParams();

  MFEMClosedCoilSource(const InputParameters & parameters);
  virtual ~MFEMClosedCoilSource() override {}

  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}

protected:
  const MFEMVariable & _source_electric_field;
  const MFEMFESpace & _hcurl_fespace;
  const MFEMFESpace & _h1_fespace;
  const MFEMCoefficient & _total_current_coef;
  const std::string _conductivity_coef_name;

  mfem::Array<int> _coil_domains;
  const int _coil_xsection_id;
  const hephaestus::InputParameters _solver_params;
};
