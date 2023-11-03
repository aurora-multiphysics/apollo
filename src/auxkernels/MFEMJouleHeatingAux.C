#include "MFEMJouleHeatingAux.h"

registerMooseObject("ApolloApp", MFEMJouleHeatingAux);

InputParameters
MFEMJouleHeatingAux::validParams()
{
  InputParameters params = MFEMAuxSolver::validParams();

  return params;
}

MFEMJouleHeatingAux::MFEMJouleHeatingAux(const InputParameters & parameters)
  : MFEMAuxSolver(parameters),
    joule_heating_params({{"CoupledVariableName", std::string("electric_field")},
                          {"ConductivityCoefName", std::string("electrical_conductivity")},
                          {"JouleHeatingVarName", std::string("joule_heating")}}),
    joule_heating_aux(joule_heating_params)
{
}

hephaestus::AuxSolver *
MFEMJouleHeatingAux::getAuxSolver()
{
  return &joule_heating_aux;
}

void
MFEMJouleHeatingAux::storeCoefficients(hephaestus::Coefficients & coefficients)
{
  std::string coef_name = std::string("JouleHeating");
  coefficients.scalars.Register(coef_name, &joule_heating_aux, true);
}

MFEMJouleHeatingAux::~MFEMJouleHeatingAux() {}
