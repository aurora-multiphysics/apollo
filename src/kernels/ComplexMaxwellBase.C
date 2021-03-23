//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

//* Base class for kernels used to solve (complex) Maxwell equations in frequency domain.
//* ComplexMaxwell:
//* ∇×(ν∇×E꜀) - εω²E꜀ = iωJ꜀
//* where
//* E꜀ is the complex electric field
//* The complex magnetic field H꜀ = (ν∇×E꜀)/iω
//* The real electric field E = Re{E꜀exp(-iωt)}
//* The real magnetic field H = Re{H꜀exp(-iωt)}
//* In weak form:꜀
//* ComplexMaxwell:
//* (ν∇×E꜀, ∇×v) - (εω²E꜀, v) - <(ν∇×E꜀)×n, v> = (iωJ꜀, v)
//* The (complex) current density can be rewritten as
//* J꜀ = σE꜀+ J꜀ᵉ
//* where J꜀ᵉ is an externally imposed current density distribution.

#include "ComplexMaxwellBase.h"
#include "Function.h"
#include "Assembly.h"

registerMooseObject("ApolloApp", ComplexMaxwellBase);

InputParameters
ComplexMaxwellBase::validParams()
{
  InputParameters params = VectorKernel::validParams();
  params.addClassDescription("This class computes various components of the"
                             "Maxwell equations which can then be assembled "
                             "together in child classes.");
  params.addRequiredCoupledVar("v", "Coupled vector variable");
  return params;
}

ComplexMaxwellBase::ComplexMaxwellBase(const InputParameters & parameters)
  : VectorKernel(parameters),
    _curl_phi(_assembly.curlPhi(_var)),
    _curl_test(_var.curlPhi()),
    _curl_u(_is_implicit ? _var.curlSln() : _var.curlSlnOld()),
    _v_id(coupled("v")),
    _v(coupledVectorValue("v")),
    _curl_v(coupledCurl("v"))
{
}

Real
ComplexMaxwellBase::curlCurlTerm()
{
  return _curl_u[_qp] * _curl_test[_i][_qp];
}

Real
ComplexMaxwellBase::dCurlCurlDU()
{
  return _curl_phi[_j][_qp] * _curl_test[_i][_qp];
}

Real
ComplexMaxwellBase::gaugePenaltyTerm()
{
  return _grad_u[_qp].tr() * _grad_test[_i][_qp].tr();
}

Real
ComplexMaxwellBase::dGaugePenaltyDU()
{
  return _grad_phi[_j][_qp].tr() * _grad_test[_i][_qp].tr();
}

Real
ComplexMaxwellBase::coupledCurlCurlTerm()
{
  return _curl_v[_qp] * _curl_test[_i][_qp];
}

Real
ComplexMaxwellBase::dCoupledCurlCurlDU()
{
  return _curl_phi[_j][_qp] * _curl_test[_i][_qp];
}

Real
ComplexMaxwellBase::computeQpResidual() {return 0;}

Real
ComplexMaxwellBase::computeQpJacobian(){return 0;}