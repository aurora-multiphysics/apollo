#include "ComplexMagnitude.h"

registerMooseObject("ApolloApp", ComplexMagnitude);

InputParameters
ComplexMagnitude::validParams()
{
  InputParameters params = VectorAuxKernel::validParams();
  params.addRequiredCoupledVar("vector_re", "The real component of the complex vector.");
  params.addRequiredCoupledVar("vector_im", "The real component of the complex vector.");
  return params;
}

ComplexMagnitude::ComplexMagnitude(const InputParameters & parameters)
  : VectorAuxKernel(parameters),
    _vec_real(coupledVectorValue("vector_re")),
    _vec_imag(coupledVectorValue("vector_im"))
{
}

RealVectorValue
ComplexMagnitude::computeValue()
{
  // Calculate the magnitude of the complex vector variable.
  return sqrt(_vec_real[_qp]*_vec_real[_qp] + _vec_imag[_qp]*_vec_imag[_qp]);
}
