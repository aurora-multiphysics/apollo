#include "MagneticField.h"

registerMooseObject("ApolloApp", MagneticField);

InputParameters
MagneticField::validParams()
{
  InputParameters params = VectorAuxKernel::validParams();
  params.addRequiredCoupledVar("electric_vector_potential", "The electric vector potential (T).");
  params.addCoupledVar("magnetic_scalar_potential",0,
                               "The magnetic scalar potential (omega).");
  return params;
}

MagneticField::MagneticField(const InputParameters & parameters)
  : VectorAuxKernel(parameters),
    _T(coupledVectorValue("electric_vector_potential")),
    _grad_omega(coupledGradient("magnetic_scalar_potential"))
{
}

RealVectorValue
MagneticField::computeValue()
{
  // Calculate the magnetic field in the TO formulation.
  return _T[_qp] + _grad_omega[_qp];
}
