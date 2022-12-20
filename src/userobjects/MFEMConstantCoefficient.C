#include "MFEMConstantCoefficient.h"

registerMooseObject("ApolloApp", MFEMConstantCoefficient);

InputParameters
MFEMConstantCoefficient::validParams()
{
  InputParameters params = MFEMCoefficient::validParams();
  params.addRequiredParam<double>("value", "Value for the ConstantCoefficient");
  return params;
}

MFEMConstantCoefficient::MFEMConstantCoefficient(const InputParameters & parameters)
  : MFEMCoefficient(parameters), mfem::ConstantCoefficient(getParam<double>("value"))
{
}

MFEMConstantCoefficient::~MFEMConstantCoefficient() {}
