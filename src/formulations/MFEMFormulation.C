#include "MFEMFormulation.h"

registerMooseObject("ApolloApp", MFEMFormulation);

InputParameters
MFEMFormulation::validParams()
{
  InputParameters params = GeneralUserObject::validParams();
  params.registerBase("MFEMFormulation");
  params.addParam<std::string>("formulation_name", "Name of Hephaestus formulation");
  return params;
}

MFEMFormulation::MFEMFormulation(const InputParameters & parameters)
  : GeneralUserObject(parameters),
    formulation_name(getParam<std::string>("formulation_name")),
    problem_builder(hephaestus::Factory::createProblemBuilder(formulation_name))
{
}

MFEMFormulation::~MFEMFormulation() {}
