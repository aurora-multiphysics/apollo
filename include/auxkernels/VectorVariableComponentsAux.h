#pragma once

#include "AuxKernel.h"

/**
 * Construct a vector variable from 3 standard variables.
 */
class VectorVariableComponentsAux : public VectorAuxKernel
{
public:
  static InputParameters validParams();

  VectorVariableComponentsAux(const InputParameters & parameters);

protected:
  virtual void compute() override final;

  // NB: not used.
  virtual RealVectorValue computeValue() override final { mooseError("Unused"); }

  /**
   * The component variables. These are set internally from the vector variable.
   */
  MooseVariable & _component_x;
  MooseVariable & _component_y;
  MooseVariable & _component_z;

private:
  /**
   * Returns a *writable* MooseVariable object for a nodal or elemental variable. Use
   * var.setNodalValue(val[, idx]) in both cases (!) to set the solution DOF values. See
   * identically-named method in "Coupleable.h". The reason for this version is that
   * the latter method refuses to work with VectorAuxKernels.
   */
  MooseVariable & writableVariable(const std::string & var_name, unsigned int comp = 0);
};
