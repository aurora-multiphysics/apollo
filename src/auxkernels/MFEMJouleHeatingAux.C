#include "MFEMJouleHeatingAux.h"
#include "Function.h"

registerMooseObject("ApolloApp", MFEMJouleHeatingAux);

InputParameters
MFEMJouleHeatingAux::validParams()
{
  InputParameters params = MFEMAuxKernel::validParams();

  return params;
}

MFEMJouleHeatingAux::MFEMJouleHeatingAux(const InputParameters & parameters)
  : MFEMAuxKernel(parameters),
    joule_heating_params({{"CoupledVariableName", std::string("electric_field")},
                          {"ConductivityCoefName", std::string("electrical_conductivity")},
                          {"JouleHeatingVarName", std::string("joule_heating")}}),
    joule_heating_aux(joule_heating_params)
{
}

hephaestus::AuxKernel *
MFEMJouleHeatingAux::getAuxKernel()
{
  return &joule_heating_aux;
}

void
MFEMJouleHeatingAux::storeCoefficients(hephaestus::DomainProperties & domain_properties)
{
  std::string coef_name = std::string("JouleHeating");
  domain_properties.scalar_property_map[coef_name] = &joule_heating_aux;
}

MFEMJouleHeatingAux::~MFEMJouleHeatingAux() {}
