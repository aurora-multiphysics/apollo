#include "HFormulation.h"

registerMooseObject("ApolloApp", HFormulation);

InputParameters
HFormulation::validParams()
{
  InputParameters params = MFEMFormulation::validParams();
  params.addRequiredParam<std::string>(
      "magnetic_field_name",
      "Name of H(curl) conforming MFEM gridfunction representing magnetic field");
  params.addRequiredParam<std::string>(
      "magnetic_permeability_name", "Name of MFEM coefficient representing magnetic permeability");
  params.addRequiredParam<std::string>(
      "electric_conductivity_name", "Name of MFEM coefficient representing electric conductivity");
  params.addRequiredParam<std::string>(
      "electric_resistivity_name",
      "Name of MFEM coefficient to be created to represent electric "
      "resistivity (reciprocal of conductivity)");
  params.addParam<std::string>(
      "current_density_name",
      "Name of H(Div) conforming MFEM gridfunction to store current density");
  params.addParam<std::string>(
      "magnetic_flux_density_name",
      "Name of H(Div) conforming MFEM gridfunction to store magnetic flux density");
  params.addParam<std::string>(
      "electric_field_name",
      "Name of H(Curl) conforming MFEM gridfunction to store electric field");
  params.addParam<std::string>(
      "lorentz_force_density_name",
      "Name of L2 conforming MFEM gridfunction to store Lorentz force density");
  params.addParam<std::string>(
      "joule_heating_density_name",
      "Name of L2 conforming MFEM gridfunction to store Joule heating density");
  params.addParam<std::string>(
      "external_current_density_name",
      "Name of H(Div) conforming MFEM gridfunction storing external current density");
  params.addParam<std::string>(
      "external_magnetic_flux_density_name",
      "Name of H(Div) conforming MFEM gridfunction storing external magnetic flux density");
  params.addParam<std::string>(
      "external_electric_field_name",
      "Name of H(Curl) conforming MFEM gridfunction storing external electric field");
  params.addParam<std::string>(
      "external_magnetic_field_name",
      "Name of H(Curl) conforming MFEM gridfunction storing external magnetic field");
  return params;
}

HFormulation::HFormulation(const InputParameters & parameters)
  : MFEMFormulation(parameters),
    _magnetic_field_name(getParam<std::string>("magnetic_field_name")),
    _magnetic_permeability_name(getParam<std::string>("magnetic_permeability_name")),
    _electric_conductivity_name(getParam<std::string>("electric_conductivity_name")),
    _electric_resistivity_name(getParam<std::string>("electric_resistivity_name")),
    _electric_field_name(
        isParamValid("electric_field_name") ? getParam<std::string>("electric_field_name") : ""),
    _current_density_name(
        isParamValid("current_density_name") ? getParam<std::string>("current_density_name") : ""),
    _magnetic_flux_density_name(isParamValid("magnetic_flux_density_name")
                                    ? getParam<std::string>("magnetic_flux_density_name")
                                    : ""),
    _lorentz_force_density_name(isParamValid("lorentz_force_density_name")
                                    ? getParam<std::string>("lorentz_force_density_name")
                                    : ""),
    _joule_heating_density_name(isParamValid("joule_heating_density_name")
                                    ? getParam<std::string>("joule_heating_density_name")
                                    : ""),
    _external_electric_field_name(isParamValid("external_electric_field_name")
                                      ? getParam<std::string>("external_electric_field_name")
                                      : ""),
    _external_magnetic_field_name(isParamValid("external_magnetic_field_name")
                                      ? getParam<std::string>("external_magnetic_field_name")
                                      : ""),
    _external_current_density_name(isParamValid("external_current_density_name")
                                       ? getParam<std::string>("external_current_density_name")
                                       : ""),
    _external_magnetic_flux_density_name(
        isParamValid("external_magnetic_flux_density_name")
            ? getParam<std::string>("external_magnetic_flux_density_name")
            : "")
{
  formulation = std::make_shared<hephaestus::HFormulation>(_electric_resistivity_name,
                                                           _electric_conductivity_name,
                                                           _magnetic_permeability_name,
                                                           _magnetic_field_name);

  if (isParamValid("electric_field_name"))
    formulation->RegisterElectricFieldAux(_electric_field_name, _external_electric_field_name);
  if (isParamValid("current_density_name"))
    formulation->RegisterCurrentDensityAux(_current_density_name, _external_current_density_name);
  if (isParamValid("magnetic_flux_density_name"))
    formulation->RegisterMagneticFluxDensityAux(_magnetic_flux_density_name,
                                                _external_magnetic_flux_density_name);

  if (isParamValid("lorentz_force_density_name"))
    formulation->RegisterLorentzForceDensityAux(
        _lorentz_force_density_name, _magnetic_flux_density_name, _current_density_name);
  if (isParamValid("joule_heating_density_name"))
    formulation->RegisterJouleHeatingDensityAux(
        _joule_heating_density_name, _electric_field_name, _current_density_name);
}

HFormulation::~HFormulation() {}
