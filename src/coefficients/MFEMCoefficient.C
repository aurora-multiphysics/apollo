#include "MFEMCoefficient.h"

registerMooseObject("ApolloApp", MFEMCoefficient);

InputParameters
MFEMCoefficient::validParams()
{
  InputParameters params = GeneralUserObject::validParams();
  return params;
}

MFEMCoefficient::MFEMCoefficient(const InputParameters & parameters) : GeneralUserObject(parameters)
{
}

MFEMCoefficient::~MFEMCoefficient() {}
