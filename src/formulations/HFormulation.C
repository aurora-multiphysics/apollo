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
  if (isParamValid("current_density_name"))
    formulation.RegisterCurrentDensityAux(getParam<std::string>("current_density_name"));
  if (isParamValid("magnetic_flux_density_name"))
    formulation.RegisterMagneticFluxDensityAux(getParam<std::string>("magnetic_flux_density_name"));
  if (isParamValid("electric_field_name"))
    formulation.RegisterElectricFieldAux(getParam<std::string>("electric_field_name"));
  if (isParamValid("lorentz_force_density_name"))
    formulation.RegisterLorentzForceDensityAux(getParam<std::string>("lorentz_force_density_name"),
                                               getParam<std::string>("magnetic_flux_density_name"),
                                               getParam<std::string>("current_density_name"));
  if (isParamValid("joule_heating_density_name"))
    formulation.RegisterJouleHeatingDensityAux(getParam<std::string>("joule_heating_density_name"),
                                               getParam<std::string>("electric_field_name"),
                                               electric_conductivity_name);
}

HFormulation::~HFormulation() {}
