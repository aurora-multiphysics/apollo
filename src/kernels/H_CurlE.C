#include "H_CurlE.h"
#include "Function.h"
#include "Assembly.h"

registerMooseObject("ApolloApp", H_CurlE);

InputParameters
H_CurlE::validParams()
{
  InputParameters params = VectorKernel::validParams();
  params.addParam<FunctionName>("x_forcing_func", 0, "The x forcing function.");
  params.addParam<FunctionName>("y_forcing_func", 0, "The y forcing function.");
  params.addParam<FunctionName>("z_forcing_func", 0, "The z forcing function.");
  return params;
}

H_CurlE::H_CurlE(const InputParameters & parameters)
  : VectorKernel(parameters),
    _curl_test(_var.curlPhi()),
    _curl_phi(_assembly.curlPhi(_var)),
    _curl_u(_is_implicit ? _var.curlSln() : _var.curlSlnOld()),
    _x_ffn(getFunction("x_forcing_func")),
    _y_ffn(getFunction("y_forcing_func")),
    _z_ffn(getFunction("z_forcing_func")),
    _resistivity(getMaterialProperty<Real>("resistivity")),
    _drhodj(getMaterialProperty<Real>("drhodj"))
{
}

Real
H_CurlE::computeQpResidual()
{
  // ec = 0.1
  // n = 10
  // jc = 1
  //(_ec[_qp] / _jc[_qp]) * pow((_j[_qp].norm() / _jc[_qp]), _n[_qp] - 1)
  // return _curl_test[_i][_qp] * 0.1 * pow(_curl_u[_qp].norm(), 9) * _curl_u[_qp] -
  //        RealVectorValue(_x_ffn.value(_t, _q_point[_qp]),
  //                        _y_ffn.value(_t, _q_point[_qp]),
  //                        _z_ffn.value(_t, _q_point[_qp])) *
  //            _test[_i][_qp];

  return _curl_test[_i][_qp] * _resistivity[_qp] * _curl_u[_qp] -
         RealVectorValue(_x_ffn.value(_t, _q_point[_qp]),
                         _y_ffn.value(_t, _q_point[_qp]),
                         _z_ffn.value(_t, _q_point[_qp])) *
             _test[_i][_qp];
}
// Jc(B) implemented like ffn ->
Real
H_CurlE::computeQpJacobian()
{
  // return _curl_test[_i][_qp] * 0.1 * 9 * pow(_curl_u[_qp].norm(), 8) * _curl_phi[_j][_qp];

  return _curl_test[_i][_qp] * _drhodj[_qp] * _curl_phi[_j][_qp];
}

