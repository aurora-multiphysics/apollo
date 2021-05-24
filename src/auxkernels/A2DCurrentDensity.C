#include "A2DCurrentDensity.h"

registerMooseObject("ApolloApp", A2DCurrentDensity);

InputParameters
A2DCurrentDensity::validParams()
{
  InputParameters params = AuxKernel::validParams();
  params.addRequiredCoupledVar("magnetic_vector_potential", "The magnetic vector potential (A).");
  return params;
}

A2DCurrentDensity::A2DCurrentDensity(const InputParameters & parameters)
  : AuxKernel(parameters),
    _a_dot(coupledDot("magnetic_vector_potential")),
    _sigma(getMaterialProperty<Real>("electrical_conductivity"))
{
}

Real
A2DCurrentDensity::computeValue()
{
  // Calculate the current density in the A formulation
  return -_sigma[_qp]*_a_dot[_qp];
}
