#include "CustomFormulation.h"

registerMooseObject("ApolloApp", CustomFormulation);

InputParameters
CustomFormulation::validParams()
{
  InputParameters params = MFEMFormulation::validParams();
  return params;
}

CustomFormulation::CustomFormulation(const InputParameters & parameters)
  : MFEMFormulation(parameters), formulation()
{
}

CustomFormulation::~CustomFormulation() {}
