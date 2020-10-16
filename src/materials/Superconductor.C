//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "Superconductor.h"
#include "Function.h"

registerMooseObject("HammerheadApp", Superconductor);

InputParameters
Superconductor::validParams()
{
  InputParameters params = Material::validParams();

  // Parameter for radius of the spheres used to interpolate permeability.
  params.addParam<Real>(
      "critical_electric_field",
      1.0,
      "The critical electric field ($E_c$) of the superconductor in arbitrary units.");
  params.addParam<Real>(
      "critical_current_density",
      1.0,
      "The critical current density ($J_c$) of the superconductor in arbitrary units.");
  params.addParam<Real>(
      "permeability", 1.0, "The permeability ($\\mu$) of the conductor. Defaults to 1");
  params.addParam<Real>("nonlinearity_parameter",
                        1.0,
                        "The nonlinearity parameter ($n$) of the superconductor. Defaults to 1, "
                        "representing a linear resistivity term");
  params.addCoupledVar("magnetic_field", "The magnetic field ($H$) as a function of position.");

  return params;
}

Superconductor::Superconductor(const InputParameters & parameters)
  : Material(parameters),
    // DerivativeMaterialInterface<Material>(parameters),

    // Get the parameters from the input file
    _input_n(getParam<Real>("nonlinearity_parameter")),
    _input_jc(getParam<Real>("critical_current_density")),
    _input_ec(getParam<Real>("critical_electric_field")),
    _input_permeability(getParam<Real>("permeability")),
    // Declare material properties
    _ec(declareProperty<Real>("critical_electric_field")),
    _jc(declareProperty<Real>("critical_current_density")),
    _n(declareProperty<Real>("nonlinearity_parameter")),
    // get the c variable value, number, and name
    // _H(coupledValue("magnetic_field")),
    // _H_var(coupled("magnetic_field")),
    // _H_name(getVar("magnetic_field", 0)->name()),
    // Declare two material properties by getting a reference from the MOOSE Material system
    _permeability(declareProperty<Real>("permeability")),
    _resistivity(declareProperty<Real>("resistivity")),
    _drhodj(declareProperty<Real>("drhodj")),
    // _drdH(declarePropertyDerivative<Real>("resistivity", _H_name)),
    // _d2rdH2(declarePropertyDerivative<Real>("resistivity", _H_name, _H_name))

    // Declare two material properties by getting a reference from the MOOSE Material system
    // _h(isCoupled("magnetic_field"),
    _j(coupledCurl("magnetic_field"))
{
}

void
Superconductor::computeQpProperties()
{
  _ec[_qp] = _input_ec;
  _jc[_qp] = _input_jc;
  _n[_qp] = _input_n;
  _permeability[_qp] = _input_permeability;
  _resistivity[_qp] = (_ec[_qp] / _jc[_qp]) * pow((_j[_qp].norm() / _jc[_qp]), _n[_qp] - 1);
  _drhodj[_qp] = (_ec[_qp] / _jc[_qp]) * _n[_qp] * (pow((_j[_qp].norm() / _jc[_qp]), _n[_qp] - 1));
}
