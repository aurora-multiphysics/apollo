#include "TimeAverageJouleHeatingAux.h"

registerMooseObject("ApolloApp", MFEMTimeAverageJouleHeatingAux);

InputParameters
MFEMTimeAverageJouleHeatingAux::validParams()
{
  InputParameters params = MFEMAuxSolver::validParams();
  return params;
}

MFEMTimeAverageJouleHeatingAux::MFEMTimeAverageJouleHeatingAux(const InputParameters & parameters)
  : MFEMAuxSolver(parameters),
    avg_joule_heating_params({{"CoupledVariableName", std::string("electric_field")},
                              {"ConductivityCoefName", std::string("electrical_conductivity")},
                              {"JouleHeatingVarName", std::string("average_joule_heating")}}),
    avg_joule_heating_aux{std::make_shared<TimeAverageJouleHeatingCoefficient>(avg_joule_heating_params)}
{
}

void
MFEMTimeAverageJouleHeatingAux::storeCoefficients(hephaestus::Coefficients & coefficients)
{
  coefficients._scalars.Register("AverageJouleHeating", avg_joule_heating_aux);
}

MFEMTimeAverageJouleHeatingAux::~MFEMTimeAverageJouleHeatingAux() {}
