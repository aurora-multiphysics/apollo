#include "Dielectric.h"
#include "Function.h"

registerMooseObject("ApolloApp", Dielectric);

InputParameters
Dielectric::validParams()
{
  InputParameters params = Material::validParams();
  params.addParam<Real>(
      "rel_permittivity", 1.0, "The relative permittivity ($\\varepsilon$) of the conductor. Defaults to 1");
  params.addParam<Real>(
      "rel_permeability", 1.0, "The relative permeability ($\\mu$) of the conductor. Defaults to 1");
  return params;
}

Dielectric::Dielectric(const InputParameters & parameters)
  : Material(parameters),
    // Get the parameters from the input file
    _input_rel_permittivity(getParam<Real>("rel_permittivity")),
    _input_rel_permeability(getParam<Real>("rel_permeability")),

    // Declare material properties by getting a reference from the MOOSE Material system
    _permittivity(declareProperty<Real>("permittivity")),
    _permeability(declareProperty<Real>("permeability")),
    _reluctance(declareProperty<Real>("reluctance"))
{
}

void
Dielectric::computeQpProperties()
{
  const Real _epsilon0 = 8.85418782e-12;
  const Real _mu0 = 1.25663706e-6;
  _permittivity[_qp] = _epsilon0 * _input_rel_permittivity;
  _permeability[_qp] = _mu0 * _input_rel_permeability;
  _reluctance[_qp] =  1.0/_permeability[_qp];
}
