#include "AVFormulation.h"

registerMooseObject("ApolloApp", AVFormulation);

InputParameters
AVFormulation::validParams()
{
  InputParameters params = MFEMFormulation::validParams();
  params.addParam<std::string>(
      "magnetic_vector_potential_name",
      "Name of H(curl) conforming MFEM gridfunction representing magnetic vector potential A");
  params.addParam<std::string>(
      "electric_potential_name",
      "Name of H1 conforming MFEM gridfunction representing electric potential V");
  params.addParam<std::string>("magnetic_permeability_name",
                               "Name of MFEM coefficient representing magnetic permeability");
  params.addParam<std::string>("electric_conductivity_name",
                               "Name of MFEM coefficient representing electric conductivity");
  params.addParam<std::string>("magnetic_reluctivity_name",
                               "Name of MFEM coefficient to be created to represent magnetic "
                               "reluctivity (reciprocal of permeability)");
  return params;
}

AVFormulation::AVFormulation(const InputParameters & parameters)
  : MFEMFormulation(parameters),
    magnetic_vector_potential_name(getParam<std::string>("magnetic_vector_potential_name")),
    electric_potential_name(getParam<std::string>("electric_potential_name")),
    magnetic_permeability_name(getParam<std::string>("magnetic_permeability_name")),
    electric_conductivity_name(getParam<std::string>("electric_conductivity_name")),
    magnetic_reluctivity_name(getParam<std::string>("magnetic_reluctivity_name"))
{
  formulation = std::make_shared<hephaestus::AVFormulation>(magnetic_reluctivity_name,
                                                            magnetic_permeability_name,
                                                            electric_conductivity_name,
                                                            magnetic_vector_potential_name,
                                                            electric_potential_name);
}

AVFormulation::~AVFormulation() {}
