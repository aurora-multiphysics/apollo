//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "Material.h"

// A helper class from MOOSE that linear interpolates x,y data
#include "LinearInterpolation.h"

/**
 * Material objects inherit from Material and override computeQpProperties.
 *
 * Their job is to declare properties for use by other objects in the
 * calculation such as Kernels and BoundaryConditions.
 */
class ComplexConductor : public Material
{
public:
  static InputParameters validParams();

  ComplexConductor(const InputParameters & parameters);

protected:
  /// Necessary override. This is where the values of the properties are computed.
  virtual void computeQpProperties() override;

  /// Value of resistivity from the input file
  const Real & _input_real_conductivity;
  const Real & _input_imag_conductivity;
  const Real & _input_real_rel_permittivity;
  const Real & _input_imag_rel_permittivity;
  const Real & _input_real_rel_permeability;
  const Real & _input_imag_rel_permeability;

  /// The resistivity (rho)
  MaterialProperty<Real> & _real_conductivity;
  MaterialProperty<Real> & _imag_conductivity;
  MaterialProperty<Real> & _real_permittivity;
  MaterialProperty<Real> & _imag_permittivity;
  MaterialProperty<Real> & _real_permeability;
  MaterialProperty<Real> & _imag_permeability;
  MaterialProperty<Real> & _real_reluctance;
  MaterialProperty<Real> & _imag_reluctance;
};
