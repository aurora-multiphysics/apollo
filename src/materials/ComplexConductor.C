//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ComplexConductor.h"
#include "Function.h"

registerMooseObject("ApolloApp", ComplexConductor);

InputParameters
ComplexConductor::validParams()
{
  InputParameters params = Material::validParams();

  // Parameter for radius of the spheres used to interpolate permeability.
  params.addParam<Real>(
      "real_conductivity", 0.0, "The real component of the conductivity ($\\sigma$) of the conductor. Defaults to 1");
  params.addParam<Real>(
      "imag_conductivity", 0.0, "The imaginary component of the conductivity ($\\sigma$) of the conductor. Defaults to 0");

  params.addParam<Real>(
      "real_rel_permittivity", 1.0, "The real component of the relative permittivity ($\\varepsilon$) of the conductor. Defaults to 1");
  params.addParam<Real>(
      "imag_rel_permittivity", 0.0, "The imaginary component of the relative permittivity ($\\varepsilon$) of the conductor. Defaults to 0");

  params.addParam<Real>(
      "real_rel_permeability", 1.0, "The real component of the relative permeability ($\\mu$) of the conductor. Defaults to 1");
  params.addParam<Real>(
      "imag_rel_permeability", 0.0, "The imaginary component of the relative permeability ($\\mu$) of the conductor. Defaults to 0");
  return params;
}

ComplexConductor::ComplexConductor(const InputParameters & parameters)
  : Material(parameters),
    // Get the parameters from the input file
    _input_real_conductivity(getParam<Real>("real_conductivity")),
    _input_imag_conductivity(getParam<Real>("imag_conductivity")),

    _input_real_rel_permittivity(getParam<Real>("real_rel_permittivity")),    
    _input_imag_rel_permittivity(getParam<Real>("imag_rel_permittivity")),
    
    _input_real_rel_permeability(getParam<Real>("real_rel_permeability")),
    _input_imag_rel_permeability(getParam<Real>("imag_rel_permeability")), 

    // Declare two material properties by getting a reference from the MOOSE Material system
    _real_conductivity(declareProperty<Real>("real_conductivity")),
    _imag_conductivity(declareProperty<Real>("imag_conductivity")),

    _real_permittivity(declareProperty<Real>("real_permittivity")),
    _imag_permittivity(declareProperty<Real>("imag_permittivity")),

    _real_permeability(declareProperty<Real>("real_permeability")),
    _imag_permeability(declareProperty<Real>("imag_permeability")),

    _real_reluctance(declareProperty<Real>("real_reluctance")),
    _imag_reluctance(declareProperty<Real>("imag_reluctance"))

{
}

void
ComplexConductor::computeQpProperties()
{
  // Real value = _radius.value(_t, _q_point[_qp]);
  // mooseAssert(value >= 1 && value <= 3,
  //             "The radius range must be in the range [1, 3], but " << value << " provided.");
  _real_conductivity[_qp] = _input_real_conductivity;
  _imag_conductivity[_qp] = _input_imag_conductivity;

  _real_permittivity[_qp] = 8.85418782e-12 * _input_real_rel_permittivity;
  _imag_permittivity[_qp] = 8.85418782e-12 * _input_imag_rel_permittivity;

  _real_permeability[_qp] = 1.25663706e-6 * _input_real_rel_permeability;
  _imag_permeability[_qp] = 1.25663706e-6 * _input_imag_rel_permeability;

  _real_reluctance[_qp] =  _real_permeability[_qp]/(1e-99 +_real_permeability[_qp]*_real_permeability[_qp]+_imag_permeability[_qp]*_imag_permeability[_qp]);
  _imag_reluctance[_qp] = -_imag_permeability[_qp]/(1e-99 +_real_permeability[_qp]*_real_permeability[_qp]+_imag_permeability[_qp]*_imag_permeability[_qp]);  
}
