#include "ElectricField.h"

registerMooseObject("ApolloApp", ElectricField);

InputParameters
ElectricField::validParams()
{
  InputParameters params = VectorAuxKernel::validParams();
  params.addRequiredCoupledVar("magnetic_vector_potential", "The magnetic vector potential (A).");
  params.addCoupledVar("electric_scalar_potential",0,
                               "The electric scalar potential (V).");
  return params;
}

ElectricField::ElectricField(const InputParameters & parameters)
  : VectorAuxKernel(parameters),
    _a_dot(coupledVectorDot("magnetic_vector_potential")),
    _grad_v(coupledGradient("electric_scalar_potential"))
{
}

RealVectorValue
ElectricField::computeValue()
{
  // Calculate the electric field in the AV formulation
  return -_a_dot[_qp] - _grad_v[_qp];
}
