//* Adds the kernel for the curl-curl term
//* curl(xi * curl u)
//*
//* in weak form:
//* (xi * curl u, curl v)

//* For T-phi formulation, u = T, p = omega, xi = rho, eta = mu
//* and H = T0 + T - grad phi = u + grad p

//* For A-V formulation, u = A, p = int(V dt), xi = mu^-1, eta = sigma
//* B = curl A

#include "CurlCurl.h"
#include "Function.h"
#include "Assembly.h"

registerMooseObject("HammerheadApp", CurlCurl);

InputParameters
CurlCurl::validParams()
{
  InputParameters params = VectorKernel::validParams();
  return params;
}

CurlCurl::CurlCurl(const InputParameters & parameters)
  : VectorKernel(parameters),
    _curl_phi(_assembly.curlPhi(_var)),
    _curl_test(_var.curlPhi()),
    _curl_u(_is_implicit ? _var.curlSln() : _var.curlSlnOld())
{
}
// xi = E/J = rho
// dxi du = dE/dJ = J drdJ +
Real
CurlCurl::computeQpResidual()
{
  return _curl_u[_qp] * _curl_test[_i][_qp];
}

Real
CurlCurl::computeQpJacobian()
{
  return _curl_phi[_j][_qp] * _curl_test[_i][_qp];
}
