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
  // Calculate the local magnetic moment from the current density.
  return 0.5 * _q_point[_qp].cross(_current_density[_qp]);
}
