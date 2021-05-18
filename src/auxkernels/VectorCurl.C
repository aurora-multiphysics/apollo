#include "VectorCurl.h"

registerMooseObject("ApolloApp", VectorCurl);

InputParameters
VectorCurl::validParams()
{
  InputParameters params = VectorAuxKernel::validParams();

  params.addRequiredCoupledVar("vector_variable", "The vector to find the curl of.");

  return params;
}

VectorCurl::VectorCurl(const InputParameters & parameters)
  : VectorAuxKernel(parameters), _vector_curl(coupledCurl("vector_variable"))
{
}

RealVectorValue
VectorCurl::computeValue()
{
  // Access the gradient of the pressure at this quadrature point, then pull out the "component" of
  // it requested (x, y or z). Note, that getting a particular component of a gradient is done using
  // the parenthesis operator.
  return _vector_curl[_qp];
}
