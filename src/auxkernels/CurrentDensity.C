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
  // Access the gradient of the pressure at this quadrature point, then pull out the "component" of
  // it requested (x, y or z). Note, that getting a particular component of a gradient is done using
  // the parenthesis operator.
  return _current_density[_qp];
}
