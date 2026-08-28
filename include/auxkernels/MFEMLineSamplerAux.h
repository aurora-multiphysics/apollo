#pragma once

#include "MFEMAuxSolver.h"
#include "MFEMVariable.h"

class MFEMLineSamplerAux : public MFEMAuxSolver
{
public:
  static InputParameters validParams();

  MFEMLineSamplerAux(const InputParameters & parameters);
  virtual ~MFEMLineSamplerAux();

  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}

  inline std::shared_ptr<hephaestus::AuxSolver> getAuxSolver() const override
  {
    return _line_sampler_aux;
  }

protected:
  const MFEMVariable & _mfem_variable;
  const Point _start_point;
  const Point _end_point;
  mfem::Vector _start_vec;
  mfem::Vector _end_vec;
  unsigned int _num_points;

  std::shared_ptr<hephaestus::LineSamplerAux> _line_sampler_aux{nullptr};
};
