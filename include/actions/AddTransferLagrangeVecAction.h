#pragma once

#include "Action.h"

/**
 * This class allows us to have a section of the input file like the
 * following to add Lagrange vectors and 3 standard Lagrange variables
 * of the same order (to allow multi-app variable transfers).
 *
 * [TransferLagrangeVec]
 * []
 */
class AddTransferLagrangeVecAction : public Action
{
public:
  static InputParameters validParams();

  AddTransferLagrangeVecAction(const InputParameters & parameters);

  virtual void act() override;
};
