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
class Dielectric : public Material
{
public:
  static InputParameters validParams();

  Dielectric(const InputParameters & parameters);

protected:
  /// Necessary override. This is where the values of the properties are computed.
  virtual void computeQpProperties() override;

  const Real & _input_rel_permittivity;
  const Real & _input_rel_permeability;

  MaterialProperty<Real> & _permittivity;
  MaterialProperty<Real> & _permeability;
  MaterialProperty<Real> & _reluctance;
};
