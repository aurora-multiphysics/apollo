#pragma once

#include "MooseObjectAction.h"
#include "MFEMProblem.h"
/**
 * This class allows us to have a section of the input file like the
 * following to add MFEM sources to the problem.
 *
 * [Source]
 * []
 */
class AddSourceAction : public MooseObjectAction
{
public:
  static InputParameters validParams();

  AddSourceAction(const InputParameters & parameters);

  virtual void act() override;
};
