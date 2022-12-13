#pragma once

#include "Function.h"

class RacetrackCoilCurrentDensity;

template <>
InputParameters validParams<Function>();

/**
 * Class that represents constant function
 */
class RacetrackCoilCurrentDensity : public Function
{
public:
  static InputParameters validParams();

  RacetrackCoilCurrentDensity(const InputParameters & parameters);

  virtual RealVectorValue vectorValue(Real t, const Point & p) const override;

protected:
  Real x0;   // Coil centre x coordinate
  Real y0;   // Coil centre y coordinate
  Real a;    // Coil thickness
  Real I0;   // Coil current in Ampere-turns
  Real S;    // Coil cross sectional area
  Real freq; // Frequency in Hz
};
