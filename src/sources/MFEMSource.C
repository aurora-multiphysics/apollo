#include "MFEMSource.h"

registerMooseObject("ApolloApp", MFEMSource);

InputParameters
MFEMSource::validParams()
{
  InputParameters params = GeneralUserObject::validParams();
  params.addParam<std::vector<SubdomainID>>(
      "block", "The list of block IDs on which the source will be applied");
  return params;
}

MFEMSource::MFEMSource(const InputParameters & parameters)
  : GeneralUserObject(parameters), blocks(getParam<std::vector<SubdomainID>>("block"))
{
}

hephaestus::Source *
MFEMSource::getSource()
{
  return _source;
}

void
MFEMSource::storeCoefficients(hephaestus::DomainProperties & domain_properties)
{
}

MFEMSource::~MFEMSource() {}
