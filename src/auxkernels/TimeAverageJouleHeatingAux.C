#include "TimeAverageJouleHeatingAux.h"

registerMooseObject("ApolloApp", MFEMTimeAverageJouleHeatingAux);

InputParameters
MFEMTimeAverageJouleHeatingAux::validParams()
{
  InputParameters params = MFEMAuxSolver::validParams();
  params.addRequiredParam<double>("time_step","Time step for averaging,must be set as the same as the time step in the simulation");
  params.addParam<double>("skip",0.0,"Time to skip before averaging starts");
  return params;
}

MFEMTimeAverageJouleHeatingAux::MFEMTimeAverageJouleHeatingAux(const InputParameters & parameters)
  : MFEMAuxSolver(parameters),
    avg_joule_heating_params({{"CoupledVariableName", std::string("electric_field")},
                              {"ConductivityCoefName", std::string("electrical_conductivity")},
                              {"JouleHeatingVarName", std::string("average_joule_heating")},
                              {"time_step", parameters.get<double>("time_step")},
                              {"skip", parameters.get<double>("skip")}}),
                              
    avg_joule_heating_aux{std::make_shared<TimeAverageJouleHeatingCoefficient>(avg_joule_heating_params)}
{
}

void
MFEMTimeAverageJouleHeatingAux::storeCoefficients(hephaestus::Coefficients & coefficients)
{
  coefficients._scalars.Register("AverageJouleHeating", avg_joule_heating_aux);
}

MFEMTimeAverageJouleHeatingAux::~MFEMTimeAverageJouleHeatingAux() {}
