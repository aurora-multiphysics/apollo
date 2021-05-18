#include "VectorCurlBC.h"
#include "Function.h"

registerMooseObject("ApolloApp", VectorCurlBC);

InputParameters
VectorCurlBC::validParams()
{
  InputParameters params = VectorIntegratedBC::validParams();
  params.addRequiredParam<FunctionName>("curl_value", "Value of field curl on the boundary.");
  return params;
}

VectorCurlBC::VectorCurlBC(const InputParameters & parameters)
  : VectorIntegratedBC(parameters), _curl_value(getFunction("curl_value"))
{
}

Real
VectorCurlBC::computeQpResidual()
{
  return _test[_i][_qp] * _normals[_qp].cross(_curl_value.vectorCurl(_t, _q_point[_qp]));
}

Real
VectorCurlBC::computeQpJacobian()
{
  return 0.0;
}
