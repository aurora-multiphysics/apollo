#pragma once
#include "MFEMCoefficient.h"
#include "auxsolvers.hpp"

class MFEMVariableDependentFunctionCoefficient : public MFEMCoefficient,
                                                 public hephaestus::CoupledCoefficient,
                                                 public std::enable_shared_from_this<MFEMVariableDependentFunctionCoefficient>
{
public:
  static InputParameters validParams();

  MFEMVariableDependentFunctionCoefficient(const InputParameters & parameters);
  virtual ~MFEMVariableDependentFunctionCoefficient();

  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}

  double Eval(mfem::ElementTransformation & trans, const mfem::IntegrationPoint & ip) override;

  std::shared_ptr<mfem::Coefficient> getCoefficient() override { return shared_from_this(); }

private:
  const Function & _func;
};
