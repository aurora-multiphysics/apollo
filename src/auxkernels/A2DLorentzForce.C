#include "A2DLorentzForce.h"

registerMooseObject("ApolloApp", A2DLorentzForce);

InputParameters
A2DLorentzForce::validParams()
{
  InputParameters params = VectorAuxKernel::validParams();
  params.addRequiredCoupledVar("current_density", "The current density (J).");
  params.addRequiredCoupledVar("magnetic_flux_density", "The magnetic flux density (B).");
  return params;
}

A2DLorentzForce::A2DLorentzForce(const InputParameters & parameters)
  : VectorAuxKernel(parameters),
    _j_field(coupledValue("current_density")),
    _b_field(coupledVectorValue("magnetic_flux_density"))
{
}

RealVectorValue
A2DLorentzForce::computeValue()
{
  // Calculate the Lorentz force density in 2D
  return {-_j_field[_qp]*_b_field[_qp](1),_j_field[_qp]*_b_field[_qp](0)};
}
