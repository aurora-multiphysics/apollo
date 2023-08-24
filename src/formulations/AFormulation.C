#include "AFormulation.h"

registerMooseObject("ApolloApp", AFormulation);

InputParameters
AFormulation::validParams()
{
  InputParameters params = MFEMFormulation::validParams();
  params.addParam<std::string>(
      "magnetic_vector_potential_name",
      "Name of H(curl) conforming MFEM gridfunction representing magnetic vector potential");
  params.addParam<std::string>("magnetic_permeability_name",
                               "Name of MFEM coefficient representing magnetic permeability");
  params.addParam<std::string>("electric_conductivity_name",
                               "Name of MFEM coefficient representing electric conductivity");
  return params;
}

AFormulation::AFormulation(const InputParameters & parameters)
  : MFEMFormulation(parameters),
    magnetic_vector_potential_name(getParam<std::string>("magnetic_vector_potential_name")),
    magnetic_permeability_name(getParam<std::string>("magnetic_permeability_name")),
    electric_conductivity_name(getParam<std::string>("electric_conductivity_name")),
    formulation()
{
}

AFormulation::~AFormulation() {}
