#pragma once

#include "Function.h"

/**
 * Class that represents constant function
 */
class TE10Real : public Function
{
public:
  static InputParameters validParams();

  TE10Real(const InputParameters & parameters);

  virtual RealVectorValue vectorValue(Real t, const Point & p) const override;

protected:
  Real kc;
  Real gamma_im;
  Real E0;
  MooseEnum propagation_direction;
};
