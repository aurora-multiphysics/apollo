#include "ComplexConductor.h"
#include "Function.h"

registerMooseObject("ApolloApp", ComplexConductor);

InputParameters
ComplexConductor::validParams()
{
  InputParameters params = Material::validParams();

  // Parameter for radius of the spheres used to interpolate permeability.
  params.addParam<Real>(
      "real_electrical_conductivity", 0.0, "The real component of the electrical_conductivity ($\\sigma$) of the conductor. Defaults to 1");
  params.addParam<Real>(
      "imag_electrical_conductivity", 0.0, "The imaginary component of the electrical_conductivity ($\\sigma$) of the conductor. Defaults to 0");

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
    _input_real_electrical_conductivity(getParam<Real>("real_electrical_conductivity")),
    _input_imag_electrical_conductivity(getParam<Real>("imag_electrical_conductivity")),

    _input_real_rel_permittivity(getParam<Real>("real_rel_permittivity")),
    _input_imag_rel_permittivity(getParam<Real>("imag_rel_permittivity")),

    _input_real_rel_permeability(getParam<Real>("real_rel_permeability")),
    _input_imag_rel_permeability(getParam<Real>("imag_rel_permeability")),

    // Declare two material properties by getting a reference from the MOOSE Material system
    _electrical_conductivity(declareProperty<Real>("electrical_conductivity")),
    _real_electrical_conductivity(declareProperty<Real>("real_electrical_conductivity")),
    _imag_electrical_conductivity(declareProperty<Real>("imag_electrical_conductivity")),

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
  //Added for auxkernel compatibility
  _electrical_conductivity[_qp] = _real_electrical_conductivity[_qp];
  _real_electrical_conductivity[_qp] = _input_real_electrical_conductivity;
  _imag_electrical_conductivity[_qp] = _input_imag_electrical_conductivity;

  _real_permittivity[_qp] = 8.85418782e-12 * _input_real_rel_permittivity;
  _imag_permittivity[_qp] = 8.85418782e-12 * _input_imag_rel_permittivity;

  _real_permeability[_qp] = 1.25663706e-6 * _input_real_rel_permeability;
  _imag_permeability[_qp] = 1.25663706e-6 * _input_imag_rel_permeability;

  _real_reluctance[_qp] =  _real_permeability[_qp]/(1e-99 +_real_permeability[_qp]*_real_permeability[_qp]+_imag_permeability[_qp]*_imag_permeability[_qp]);
  _imag_reluctance[_qp] = -_imag_permeability[_qp]/(1e-99 +_real_permeability[_qp]*_real_permeability[_qp]+_imag_permeability[_qp]*_imag_permeability[_qp]);

}
