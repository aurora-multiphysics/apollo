#include "AVCurrentDensity.h"

registerMooseObject("ApolloApp", AVCurrentDensity);

InputParameters
AVCurrentDensity::validParams()
{
  InputParameters params = VectorAuxKernel::validParams();
  params.addRequiredCoupledVar("magnetic_vector_potential", "The magnetic vector potential (A).");
  params.addCoupledVar("electric_scalar_potential",0,
                               "The electric scalar potential (V).");
  return params;
}

AVCurrentDensity::AVCurrentDensity(const InputParameters & parameters)
  : VectorAuxKernel(parameters),
    _a_dot(coupledVectorDot("magnetic_vector_potential")),
    _grad_v(coupledGradient("electric_scalar_potential")),
    _sigma(getMaterialProperty<Real>("electrical_conductivity"))
{
}

RealVectorValue
AVCurrentDensity::computeValue()
{
  // Calculate the current density in the AV formulation
  return -_sigma[_qp]*(_a_dot[_qp] + _grad_v[_qp]);
}
