#pragma once
#include "MFEMCoefficient.h"
#include "auxsolvers.hpp"

class MFEMVariableDependentFunctionCoefficient : public MFEMCoefficient,
                                                 public hephaestus::CoupledCoefficient
{
public:
  static InputParameters validParams();

  MFEMVariableDependentFunctionCoefficient(const InputParameters & parameters);
  virtual ~MFEMVariableDependentFunctionCoefficient();

  virtual void execute() override{};
  virtual void initialize() override{};
  virtual void finalize() override{};

  double Eval(mfem::ElementTransformation & trans, const mfem::IntegrationPoint & ip) override;

  virtual mfem::Coefficient * getCoefficient() override { return this; };

private:
  const Function & _func;
};
