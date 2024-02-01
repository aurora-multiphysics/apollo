#pragma once
#include "MFEMSource.h"
#include "MFEMVariable.h"
#include "MFEMCoefficient.h"

class MFEMOpenCoilSource : public MFEMSource
{
public:
  static InputParameters validParams();

  MFEMOpenCoilSource(const InputParameters & parameters);
  virtual ~MFEMOpenCoilSource() override {}

  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}

protected:
  const MFEMVariable & _source_electric_field_gridfunction;
  const MFEMVariable & _source_potential_gridfunction;
  const MFEMCoefficient & _total_current_coef;
  const std::string _conductivity_coef_name;

  const hephaestus::InputParameters _solver_params;
  mfem::Array<int> _coil_domains;
  const int _coil_in_id, _coil_out_id;
  const std::pair<int, int> _electrodes;
};
