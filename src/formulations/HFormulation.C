#include "HFormulation.h"

registerMooseObject("ApolloApp", HFormulation);

InputParameters
HFormulation::validParams()
{
  InputParameters params = MFEMFormulation::validParams();
  params.addParam<std::string>(
      "magnetic_field_name",
      "Name of H(curl) conforming MFEM gridfunction representing magnetic field");
  params.addParam<std::string>("magnetic_permeability_name",
                               "Name of MFEM coefficient representing magnetic permeability");
  params.addParam<std::string>("electric_conductivity_name",
                               "Name of MFEM coefficient representing electric conductivity");
  params.addParam<std::string>("electric_resistivity_name",
                               "Name of MFEM coefficient to be created to represent electric "
                               "resistivity (reciprocal of conductivity)");
  return params;
}

HFormulation::HFormulation(const InputParameters & parameters)
  : MFEMFormulation(parameters),
    magnetic_field_name(getParam<std::string>("magnetic_field_name")),
    magnetic_permeability_name(getParam<std::string>("magnetic_permeability_name")),
    electric_conductivity_name(getParam<std::string>("electric_conductivity_name")),
    electric_resistivity_name(getParam<std::string>("electric_resistivity_name")),
    formulation(electric_resistivity_name,
                electric_conductivity_name,
                magnetic_permeability_name,
                magnetic_field_name)
{
}

HFormulation::~HFormulation() {}
