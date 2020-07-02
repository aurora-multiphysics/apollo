// //* This file is part of the MOOSE framework
// //* https://www.mooseframework.org
// //*
// //* All rights reserved, see COPYRIGHT for full restrictions
// //* https://github.com/idaholab/moose/blob/master/COPYRIGHT
// //*
// //* Licensed under LGPL 2.1, please see LICENSE for details
// //* https://www.gnu.org/licenses/lgpl-2.1.html

// #include "Superconductor.h"
// #include "Function.h"

// registerMooseObject("HammerheadApp", Superconductor);

// InputParameters
// Superconductor::validParams()
// {
//   InputParameters params = Material::validParams();

//   // Parameter for radius of the spheres used to interpolate permeability.
//   params.addParam<FunctionName>("critical_current_density",
//                                 "1.0",
//                                 "The critical current density ($J_c$) of the superconductor in
//                                 arbitrary units.");
//   params.addParam<Real>("nonlinearity_parameter",
//                         "1.0",
//                         "The nonlinearity parameter ($n$) of the superconductor. Defaults to 1, "
//                         "representing a linear resistivity term");

//   return params;
// }

// Superconductor::Superconductor(const InputParameters & parameters)
//   : Material(parameters),

//     // Get the parameters from the input file
//     _critical_current_density(getFunction("critical_current_density")),
//     _nonlinearity_parameter(getParam<Real>("nonlinearity_parameter")),

//     // Declare two material properties by getting a reference from the MOOSE Material system
//     _permeability(declareProperty<Real>("permeability")),
//     _viscosity(declareADProperty<Real>("viscosity"))
// {
//   // From the paper: Table 1
//   std::vector<Real> sphere_sizes = {1, 3};
//   std::vector<Real> permeability = {0.8451e-9, 8.968e-9};

//   // Set the x,y data on the LinearInterpolation object.
//   _permeability_interpolation.setData(sphere_sizes, permeability);
// }

// void
// Superconductor::computeQpProperties()
// {
//   Real value = _radius.value(_t, _q_point[_qp]);
//   mooseAssert(value >= 1 && value <= 3,
//               "The radius range must be in the range [1, 3], but " << value << " provided.");

//   _resistivity[_qp] = _input_viscosity;
//   _permeability[_qp] = _permeability_interpolation.sample(value);
// }
