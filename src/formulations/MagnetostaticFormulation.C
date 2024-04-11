#include "MagnetostaticFormulation.h"

registerMooseObject("ApolloApp", MagnetostaticFormulation);

InputParameters
MagnetostaticFormulation::validParams()
{
  InputParameters params = MFEMFormulation::validParams();
  params.addRequiredParam<std::string>(
      "magnetic_vector_potential_name",
      "Name of H(curl) conforming MFEM gridfunction representing magnetic vector potential");
  params.addRequiredParam<std::string>(
      "magnetic_permeability_name", "Name of MFEM coefficient representing magnetic permeability");
  params.addRequiredParam<std::string>(
      "magnetic_reluctivity_name",
      "Name of MFEM coefficient to be created to represent magnetic "
      "reluctivity (reciprocal of permeability)");
  params.addParam<std::string>(
      "magnetic_flux_density_name",
      "Name of H(Div) conforming MFEM gridfunction to store magnetic flux density");
  params.addParam<std::string>(
      "magnetic_field_name",
      "Name of H(Curl) conforming MFEM gridfunction to store magnetic field");
  params.addParam<std::string>(
      "lorentz_force_density_name",
      "Name of L2 conforming MFEM gridfunction to store Lorentz force density");
  params.addParam<std::string>(
      "external_current_density_name",
      "Name of H(Div) conforming MFEM gridfunction storing external current density");
  params.addParam<std::string>(
      "external_magnetic_flux_density_name",
      "Name of H(Div) conforming MFEM gridfunction storing external magnetic flux density");
  params.addParam<std::string>(
      "external_magnetic_field_name",
      "Name of H(Curl) conforming MFEM gridfunction storing external magnetic field");

  return params;
}

MagnetostaticFormulation::MagnetostaticFormulation(const InputParameters & parameters)
  : MFEMFormulation(parameters),
    _magnetic_vector_potential_name(getParam<std::string>("magnetic_vector_potential_name")),
    _magnetic_permeability_name(getParam<std::string>("magnetic_permeability_name")),
    _magnetic_reluctivity_name(getParam<std::string>("magnetic_reluctivity_name")),
    _magnetic_field_name(
        isParamValid("magnetic_field_name") ? getParam<std::string>("magnetic_field_name") : ""),
    _magnetic_flux_density_name(isParamValid("magnetic_flux_density_name")
                                    ? getParam<std::string>("magnetic_flux_density_name")
                                    : ""),
    _lorentz_force_density_name(isParamValid("lorentz_force_density_name")
                                    ? getParam<std::string>("lorentz_force_density_name")
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
  formulation = std::make_shared<hephaestus::MagnetostaticFormulation>(
      _magnetic_reluctivity_name, _magnetic_permeability_name, _magnetic_vector_potential_name);

  if (isParamValid("magnetic_field_name"))
    formulation->RegisterElectricFieldAux(_magnetic_field_name, _external_magnetic_field_name);
  if (isParamValid("magnetic_flux_density_name"))
    formulation->RegisterMagneticFluxDensityAux(_magnetic_flux_density_name,
                                                _external_magnetic_flux_density_name);

  if (isParamValid("lorentz_force_density_name"))
    formulation->RegisterLorentzForceDensityAux(
        _lorentz_force_density_name, _magnetic_flux_density_name, _external_current_density_name);
}

MagnetostaticFormulation::~MagnetostaticFormulation() {}
