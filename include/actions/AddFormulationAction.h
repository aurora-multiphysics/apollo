#pragma once

#include "MooseObjectAction.h"
#include "MFEMProblem.h"
/**
 * This class allows us to have a section of the input file like the
 * following which automatically adds variables, kernels, aux kernels, bcs
 * for setting up the full wave complex Maxwell equations.
 *
 * [ComplexMaxwell]
 * []
 */
class AddFormulationAction : public MooseObjectAction
{
public:
  static InputParameters validParams();

  AddFormulationAction(const InputParameters & parameters);

  virtual void act() override;
};
