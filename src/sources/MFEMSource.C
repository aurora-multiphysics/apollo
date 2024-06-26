#include "MFEMSource.h"

registerMooseObject("ApolloApp", MFEMSource);

InputParameters
MFEMSource::validParams()
{
  InputParameters params = GeneralUserObject::validParams();
  params.registerBase("MFEMSource");
  params.addParam<std::vector<SubdomainID>>(
      "block", "The list of block IDs on which the source will be applied");
  return params;
}

MFEMSource::MFEMSource(const InputParameters & parameters)
  : GeneralUserObject(parameters), blocks(getParam<std::vector<SubdomainID>>("block"))
{
}

void
MFEMSource::storeCoefficients(hephaestus::Coefficients & coefficients)
{
}

MFEMSource::~MFEMSource() {}
