#pragma once
#include "MFEMSource.h"
#include "MFEMVariable.h"
#include "MFEMCoefficient.h"

class MFEMOpenCoilSource : public MFEMSource
{
public:
  static InputParameters validParams();

  MFEMOpenCoilSource(const InputParameters & parameters);
  virtual ~MFEMOpenCoilSource();

  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}

  virtual hephaestus::Source * getSource() override;
  virtual void storeCoefficients(hephaestus::Coefficients & coefficients) override;

protected:
  const MFEMVariable & _source_current_density_gridfunction;
  const MFEMVariable & _source_potential_gridfunction;
  const MFEMCoefficient & _total_current_coef;
  const std::string _conductivity_coef_name;

  const hephaestus::InputParameters _open_coil_params;
  mfem::Array<int> _coil_domains;
  const int _coil_in_id, _coil_out_id;
  const std::pair<int, int> _electrodes;
};
