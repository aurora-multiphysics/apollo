// //* This file is part of the MOOSE framework
// //* https://www.mooseframework.org
// //*
// //* All rights reserved, see COPYRIGHT for full restrictions
// //* https://github.com/idaholab/moose/blob/master/COPYRIGHT
// //*
// //* Licensed under LGPL 2.1, please see LICENSE for details
// //* https://www.gnu.org/licenses/lgpl-2.1.html

// #pragma once

// #include "Material.h"

// // A helper class from MOOSE that linear interpolates x,y data
// #include "LinearInterpolation.h"

// /**
//  * Material objects inherit from Material and override computeQpProperties.
//  *
//  * Their job is to declare properties for use by other objects in the
//  * calculation such as Kernels and BoundaryConditions.
//  */
// class Superconductor : public Material
// {
// public:
//   static InputParameters validParams();

//   Superconductor(const InputParameters & parameters);

// protected:
//   /// Necessary override. This is where the values of the properties are computed.
//   virtual void computeQpProperties() override;

//   /// The critical current density of the superconductor (Jc)
//   const Function & _critical_current_density;

//   /// Nonlinearity parameter (n)
//   const Real & _nonlinearity_parameter;

//   /// The resistivity (rho)
//   MaterialProperty<Real> & _resistivity;
// };
