#include "EFormulation.h"

registerMooseObject("ApolloApp", EFormulation);

InputParameters
EFormulation::validParams()
{
  InputParameters params = MFEMFormulation::validParams();
  params.addParam<std::string>(
      "e_field_name", "Name of H(curl) conforming MFEM gridfunction representing electric field");
  params.addParam<std::string>("magnetic_permeability_name",
                               "Name of MFEM coefficient representing magnetic permeability");
  params.addParam<std::string>("electric_conductivity_name",
                               "Name of MFEM coefficient representing electric conductivity");
  params.addParam<std::string>("magnetic_reluctivity_name",
                               "Name of MFEM coefficient to be created to represent magnetic "
                               "reluctivity (reciprocal of permeability)");
  return params;
}

EFormulation::EFormulation(const InputParameters & parameters)
  : MFEMFormulation(parameters),
    e_field_name(getParam<std::string>("e_field_name")),
    magnetic_permeability_name(getParam<std::string>("magnetic_permeability_name")),
    electric_conductivity_name(getParam<std::string>("electric_conductivity_name")),
    magnetic_reluctivity_name(getParam<std::string>("magnetic_reluctivity_name")),
    formulation(magnetic_reluctivity_name,
                magnetic_permeability_name,
                electric_conductivity_name,
                e_field_name)
{
}

EFormulation::~EFormulation() {}