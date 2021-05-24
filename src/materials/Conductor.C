#include "Conductor.h"
#include "Function.h"

registerMooseObject("ApolloApp", Conductor);

InputParameters
Conductor::validParams()
{
  InputParameters params = Material::validParams();

  // Parameter for radius of the spheres used to interpolate permeability.
  params.addParam<Real>(
      "electrical_conductivity", 0.0, "The electrical_conductivity ($\\sigma$) of the conductor. Defaults to 1");
  params.addParam<Real>(
      "rel_permittivity", 1.0, "The relative permittivity ($\\varepsilon$) of the conductor. Defaults to 1");
  params.addParam<Real>(
      "rel_permeability", 1.0, "The relative permeability ($\\mu$) of the conductor. Defaults to 1");
  return params;
}

Conductor::Conductor(const InputParameters & parameters)
  : Material(parameters),
    // Get the parameters from the input file
    _input_electrical_conductivity(getParam<Real>("electrical_conductivity")),
    _input_rel_permittivity(getParam<Real>("rel_permittivity")),
    _input_rel_permeability(getParam<Real>("rel_permeability")),

    // Declare material properties by getting a reference from the MOOSE Material system
    _electrical_conductivity(declareProperty<Real>("electrical_conductivity")),
    _permittivity(declareProperty<Real>("permittivity")),
    _permeability(declareProperty<Real>("permeability")),
    _resistivity(declareProperty<Real>("resistivity")),
    _reluctivity(declareProperty<Real>("reluctivity"))
{
}

void
Conductor::computeQpProperties()
{
  const Real _epsilon0 = 8.85418782e-12;
  const Real _mu0 = 1.25663706e-6;
  _electrical_conductivity[_qp] = _input_electrical_conductivity;
  _permittivity[_qp] = _epsilon0 * _input_rel_permittivity;
  _permeability[_qp] = _mu0 * _input_rel_permeability;
  _resistivity[_qp] =  1.0/_electrical_conductivity[_qp];
  _reluctivity[_qp] =  1.0/_permeability[_qp];
}
