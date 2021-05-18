#include "JouleHeating.h"

registerMooseObject("ApolloApp", JouleHeating);

InputParameters
JouleHeating::validParams()
{
  InputParameters params = AuxKernel::validParams();

  params.addRequiredCoupledVar("electric_field", "The magnitude of the local electric field (E).");

  return params;
}

JouleHeating::JouleHeating(const InputParameters & parameters)
  : AuxKernel(parameters),
  _e_field(coupledVectorValue("electric_field")),
  _sigma(getMaterialProperty<Real>("electrical_conductivity"))
{
}

Real
JouleHeating::computeValue()
{
  // Access the gradient of the pressure at this quadrature point, then pull out the "component" of
  // it requested (x, y or z). Note, that getting a particular component of a gradient is done using
  // the parenthesis operator.
  return _sigma[_qp]*_e_field[_qp]*_e_field[_qp];
}
