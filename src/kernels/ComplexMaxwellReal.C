//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

//* Solves real component of (complex) Maxwell equations in frequency domain.
//* ComplexMaxwellImag:
//* Re{∇×(ν∇×E꜀) - εω²E꜀ - iωJ}꜀ = 0
//* where
//* E꜀ is the complex electric field
//* The complex magnetic field H꜀ = (ν∇×E꜀)/iω
//* The real electric field E = Re{E꜀exp(-iωt)}
//* The real magnetic field H = Re{H꜀exp(-iωt)}
//* In weak form:꜀
//* ComplexMaxwellReal:
//* Re{(ν∇×E꜀, ∇×v) - (εω²E꜀, v) - <(ν∇×E꜀)×n, v> - (iωJ꜀, v)} = 0
//* The (complex) current density can be rewritten as
//* J꜀ = σE꜀+ J꜀ᵉ
//* where J꜀ᵉ is an externally imposed current density distribution.
//* u = Re(E), v = Im(E)

#include "ComplexMaxwellReal.h"
#include "Function.h"
#include "Assembly.h"

registerMooseObject("ApolloApp", ComplexMaxwellReal);

InputParameters
ComplexMaxwellReal::validParams()
{
  InputParameters params = ComplexMaxwellBase::validParams();
  params.addClassDescription("This class computes various components of the"
                             "Maxwell equations which can then be assembled "
                             "together in child classes.");
  params.addRequiredParam<UserObjectName>("waveguide_properties",
                                          "The name of the user object for waveguide properties");
  return params;
}

ComplexMaxwellReal::ComplexMaxwellReal(const InputParameters & parameters)
  : ComplexMaxwellBase(parameters),
  _omega(getUserObject<WaveguideProperties>("waveguide_properties")._omega),
  _sigma_re(getMaterialProperty<Real>("real_electrical_conductivity")),
  _sigma_im(getMaterialProperty<Real>("imag_electrical_conductivity")),
  _epsilon_re(getMaterialProperty<Real>("real_permittivity")),
  _epsilon_im(getMaterialProperty<Real>("imag_permittivity")),
  _nu_re(getMaterialProperty<Real>("real_reluctivity")),
  _nu_im(getMaterialProperty<Real>("imag_reluctivity"))
{
}

Real
ComplexMaxwellReal::computeQpResidual()
{
  Real _epsilon_re_eff =  _epsilon_re[_qp] - _sigma_im[_qp]/_omega;
  Real _epsilon_im_eff =  _epsilon_im[_qp] + _sigma_re[_qp]/_omega;
  return _nu_re[_qp]*ComplexMaxwellBase::curlCurlTerm()
       - _nu_im[_qp]*ComplexMaxwellBase::coupledCurlCurlTerm()
       -_omega*_omega*(_epsilon_re_eff*_u[_qp]-_epsilon_im_eff*_v[_qp])*_test[_i][_qp];
}

Real
ComplexMaxwellReal::computeQpJacobian()
{
  Real _epsilon_re_eff =  _epsilon_re[_qp] - _sigma_im[_qp]/_omega;
  Real _epsilon_im_eff =  _epsilon_im[_qp] + _sigma_re[_qp]/_omega;
  return _nu_re[_qp]*ComplexMaxwellBase::dCurlCurlDU()
       -_omega*_omega*_epsilon_re_eff*_phi[_j][_qp]*_test[_i][_qp];
}

Real
ComplexMaxwellReal::computeQpOffDiagJacobian(unsigned int jvar)
{
  Real _epsilon_re_eff =  _epsilon_re[_qp] - _sigma_im[_qp]/_omega;
  Real _epsilon_im_eff =  _epsilon_im[_qp] + _sigma_re[_qp]/_omega;
  if (jvar == _v_id)
  return - _nu_im[_qp]*ComplexMaxwellBase::dCoupledCurlCurlDU()
       +_omega*_omega*_epsilon_im_eff*_phi[_j][_qp]*_test[_i][_qp];
  return 0.0;
}