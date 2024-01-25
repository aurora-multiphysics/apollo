#include "EBFormulation.h"

registerMooseObject("ApolloApp", EBFormulation);

InputParameters
EBFormulation::validParams()
{
  InputParameters params = MFEMFormulation::validParams();
  params.addRequiredParam<std::string>(
      "e_field_name", "Name of H(curl) conforming MFEM gridfunction representing electric field");
  params.addRequiredParam<std::string>(
      "b_field_name",
      "Name of H(div) conforming MFEM gridfunction representing magnetic flux density");
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
      "lorentz_force_density_name",
      "Name of L2 conforming MFEM gridfunction to store Lorentz force density");
  params.addParam<std::string>(
      "joule_heating_density_name",
      "Name of L2 conforming MFEM gridfunction to store Joule heating density");
  return params;
}

EBFormulation::EBFormulation(const InputParameters & parameters)
  : MFEMFormulation(parameters),
    e_field_name(getParam<std::string>("e_field_name")),
    b_field_name(getParam<std::string>("b_field_name")),
    magnetic_permeability_name(getParam<std::string>("magnetic_permeability_name")),
    electric_conductivity_name(getParam<std::string>("electric_conductivity_name")),
    magnetic_reluctivity_name(getParam<std::string>("magnetic_reluctivity_name")),
    formulation(magnetic_reluctivity_name,
                magnetic_permeability_name,
                electric_conductivity_name,
                e_field_name,
                b_field_name)
{
  if (isParamValid("current_density_name"))
    formulation.RegisterCurrentDensityAux(getParam<std::string>("current_density_name"));
  if (isParamValid("lorentz_force_density_name"))
    formulation.RegisterLorentzForceDensityAux(getParam<std::string>("lorentz_force_density_name"),
                                               b_field_name,
                                               getParam<std::string>("current_density_name"));
  if (isParamValid("joule_heating_density_name"))
    formulation.RegisterJouleHeatingDensityAux(getParam<std::string>("joule_heating_density_name"),
                                               e_field_name,
                                               electric_conductivity_name);
}

EBFormulation::~EBFormulation() {}
