#include "CurrentDensity.h"

registerMooseObject("ApolloApp", CurrentDensity);

InputParameters
CurrentDensity::validParams()
{
  InputParameters params = VectorAuxKernel::validParams();

  params.addRequiredCoupledVar("magnetic_field", "The magnetic field (H).");

  return params;
}

CurrentDensity::CurrentDensity(const InputParameters & parameters)
  : VectorAuxKernel(parameters), _current_density(coupledCurl("magnetic_field"))
{
}

RealVectorValue
CurrentDensity::computeValue()
{
  // Return the current density.
  return _current_density[_qp];
}
