#include "MFEMCoefficient.h"

registerMooseObject("ApolloApp", MFEMCoefficient);

InputParameters
MFEMCoefficient::validParams()
{
  InputParameters params = GeneralUserObject::validParams();
  params.registerBase("MFEMCoefficient");
  return params;
}

MFEMCoefficient::MFEMCoefficient(const InputParameters & parameters) : GeneralUserObject(parameters)
{
}

MFEMCoefficient::~MFEMCoefficient() {}
