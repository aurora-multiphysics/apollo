//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "Conductor.h"
#include "Function.h"

registerMooseObject("HammerheadApp", Conductor);

InputParameters
Conductor::validParams()
{
  InputParameters params = Material::validParams();

  // Parameter for radius of the spheres used to interpolate permeability.
  params.addParam<Real>(
      "resistivity", 1.0, "The resistivity ($\\rho$) of the conductor. Defaults to 1");
  return params;
}

Conductor::Conductor(const InputParameters & parameters)
  : Material(parameters),

    // Get the parameters from the input file
    _input_resistivity(getParam<Real>("resistivity")),

    // Declare two material properties by getting a reference from the MOOSE Material system
    _resistivity(declareProperty<Real>("resistivity")),
    _drhodj(declareProperty<Real>("drhodj"))
{
}

void
Conductor::computeQpProperties()
{
  // Real value = _radius.value(_t, _q_point[_qp]);
  // mooseAssert(value >= 1 && value <= 3,
  //             "The radius range must be in the range [1, 3], but " << value << " provided.");
  _resistivity[_qp] = _input_resistivity;
  _drhodj[_qp] = _resistivity[_qp];
}
