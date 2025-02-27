#include "TimeAverageJouleHeatingAux.h"


// Register the object with Apollo's Factory system (MOOSE Object under the ApolloApp)
registerMooseObject("ApolloApp", TimeAverageJouleHeatingAux);

InputParameters
TimeAverageJouleHeatingAux::validParams() //valid input parameters for the Aux Kernel
{
  InputParameters params = MFEMAuxSolver::validParams(); //inherit from MFEMAuxSolver
  params.addParam<Real>("skip", 0.0, "Time interval after which averaging starts");
  params.addParam<bool>("average", true, "Whether to take the time average");

  return params;
}

TimeAverageJouleHeatingAux::TimeAverageJouleHeatingAux(const InputParameters & parameters)
  : MFEMAuxSolver(parameters),
    time_avg_joule_heating_params({{"CoupledVariableName", std::string("electric_field")},
                                   {"ConductivityCoefName", std::string("electrical_conductivity")},
                                   {"JouleHeatingVarName", std::string("avg_joule_heating")},
                                   {"Skip", parameters.get<Real>("skip")},
                                   {"Average", parameters.get<bool>("average")}}),
    time_avg_joule_heating_aux{std::make_shared<TimeAverageJouleHeatingCoefficient>(time_avg_joule_heating_params)}
{
}
// register the JouleHeatingCoefficient with the Hephaestous coefficients registry
void
TimeAverageJouleHeatingAux::storeCoefficients(hephaestus::Coefficients & coefficients)
{
  coefficients._scalars.Register("AvgJouleHeating", time_avg_joule_heating_aux);
}

//Destructor
TimeAverageJouleHeatingAux::~TimeAverageJouleHeatingAux() {}

// Real
// TimeAverageJouleHeatingAux::computeValue()
// {
//   Real joule_heat = _sigma * _electric_field[_qp] * _electric_field[_qp];

//   // Time-averaging logic
//   Real weight = (_t > _skip) ? (_avg ? _dt / (_t - _skip) : 1) : 0;
//   return (1 - weight) * _u[_qp] + weight * joule_heat;us::CoupledCoefficient).
// }