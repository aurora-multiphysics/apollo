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
  const Real & _input_real_electrical_conductivity;
  const Real & _input_imag_electrical_conductivity;
  const Real & _input_real_rel_permittivity;
  const Real & _input_imag_rel_permittivity;
  const Real & _input_real_rel_permeability;
  const Real & _input_imag_rel_permeability;

  /// The resistivity (rho)
  MaterialProperty<Real> & _electrical_conductivity;
  MaterialProperty<Real> & _real_electrical_conductivity;
  MaterialProperty<Real> & _imag_electrical_conductivity;
  MaterialProperty<Real> & _real_permittivity;
  MaterialProperty<Real> & _imag_permittivity;
  MaterialProperty<Real> & _real_permeability;
  MaterialProperty<Real> & _imag_permeability;
  MaterialProperty<Real> & _real_reluctivity;
  MaterialProperty<Real> & _imag_reluctivity;
};
