#include "EFormulation.h"

registerMooseObject("ApolloApp", EFormulation);

InputParameters
EFormulation::validParams()
{
  InputParameters params = MFEMFormulation::validParams();
  params.addRequiredParam<std::string>(
      "e_field_name", "Name of H(curl) conforming MFEM gridfunction representing electric field");
  params.addRequiredParam<std::string>(
      "magnetic_permeability_name", "Name of MFEM coefficient representing magnetic permeability");
  params.addRequiredParam<std::string>(
      "electric_conductivity_name", "Name of MFEM coefficient representing electric conductivity");
  params.addRequiredParam<std::string>(
      "magnetic_reluctivity_name",
      "Name of MFEM coefficient to be created to represent magnetic "
      "reluctivity (reciprocal of permeability)");
  params.addParam<std::string>(
      "current_density_name",
      "Name of H(Div) conforming MFEM gridfunction to store current density");
  params.addParam<std::string>(
      "joule_heating_density_name",
      "Name of L2 conforming MFEM gridfunction to store Joule heating density");
  params.addParam<std::string>(
      "external_current_density_name",
      "Name of H(Div) conforming MFEM gridfunction storing external current density");
  return params;
}

EFormulation::EFormulation(const InputParameters & parameters)
  : MFEMFormulation(parameters),
    _electric_field_name(isParamValid("e_field_name") ? getParam<std::string>("e_field_name") : ""),
    _magnetic_permeability_name(getParam<std::string>("magnetic_permeability_name")),
    _electric_conductivity_name(getParam<std::string>("electric_conductivity_name")),
    _magnetic_reluctivity_name(getParam<std::string>("magnetic_reluctivity_name")),
    _current_density_name(
        isParamValid("current_density_name") ? getParam<std::string>("current_density_name") : ""),
    _joule_heating_density_name(isParamValid("joule_heating_density_name")
                                    ? getParam<std::string>("joule_heating_density_name")
                                    : ""),
    _external_current_density_name(isParamValid("external_current_density_name")
                                       ? getParam<std::string>("external_current_density_name")
                                       : "")
{
  formulation = std::make_shared<hephaestus::EFormulation>(_magnetic_reluctivity_name,
                                                           _magnetic_permeability_name,
                                                           _electric_conductivity_name,
                                                           _electric_field_name);

  if (isParamValid("current_density_name"))
    formulation->RegisterCurrentDensityAux(_current_density_name, _external_current_density_name);

  if (isParamValid("joule_heating_density_name"))
    formulation->RegisterJouleHeatingDensityAux(_joule_heating_density_name, _electric_field_name);
}

EFormulation::~EFormulation() {}
