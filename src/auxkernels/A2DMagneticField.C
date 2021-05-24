#include "A2DMagneticField.h"

registerMooseObject("ApolloApp", A2DMagneticField);

InputParameters
A2DMagneticField::validParams()
{
  InputParameters params = VectorAuxKernel::validParams();
  params.addRequiredCoupledVar("magnetic_vector_potential", "The magnetic vector potential (A).");
  return params;
}

A2DMagneticField::A2DMagneticField(const InputParameters & parameters)
  : VectorAuxKernel(parameters),
    _grad_a(coupledGradient("magnetic_vector_potential")),
    _sigma(getMaterialProperty<Real>("electrical_conductivity"))
{
}

RealVectorValue
A2DMagneticField::computeValue()
{
  // Calculate curl (A) when A is only in the z direction.
  return {_grad_a[_qp](1), -_grad_a[_qp](0)};
}
