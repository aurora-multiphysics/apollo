#include "MagneticMoment.h"

registerMooseObject("ApolloApp", MagneticMoment);

InputParameters
MagneticMoment::validParams()
{
  InputParameters params = VectorAuxKernel::validParams();

  params.addRequiredCoupledVar("magnetic_field", "The magnetic field (H).");

  return params;
}

MagneticMoment::MagneticMoment(const InputParameters & parameters)
  : VectorAuxKernel(parameters), _current_density(coupledCurl("magnetic_field"))
{
}

RealVectorValue
MagneticMoment::computeValue()
{
  // Access the gradient of the pressure at this quadrature point, then pull out the "component" of
  // it requested (x, y or z). Note, that getting a particular component of a gradient is done using
  // the parenthesis operator.
  return 0.5 * _q_point[_qp].cross(_current_density[_qp]);
}
