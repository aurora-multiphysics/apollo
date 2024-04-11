#pragma once
#include "MFEMCoefficient.h"
#include "ApolloUtils.h"
#include "auxsolvers.hpp"

class MFEMTemperatureDependentConductivityCoefficient
  : public MFEMCoefficient,
    public hephaestus::CoupledCoefficient,
    public std::enable_shared_from_this<MFEMTemperatureDependentConductivityCoefficient>
{
public:
  static InputParameters validParams();

  MFEMTemperatureDependentConductivityCoefficient(const InputParameters & parameters);
  virtual ~MFEMTemperatureDependentConductivityCoefficient();

  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}

  double Eval(mfem::ElementTransformation & trans, const mfem::IntegrationPoint & ip) override;

  std::shared_ptr<mfem::Coefficient> getCoefficient() const override
  {
    return ApolloUtils::dynamic_const_cast<mfem::Coefficient>(getSharedPtr());
  }

private:
  const Function & _func;
};
