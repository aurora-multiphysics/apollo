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
  // Return the curl of the input vector
  return _vector_curl[_qp];
}
