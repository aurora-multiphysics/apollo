#pragma once

#include "AuxKernel.h"

/**
 * This class is required to circumvent an issue with the VectorAuxKernel class.
 * The issue was that it was not possible to call the "writableVariable" method.
 */
class ApolloVectorAuxKernel : public VectorAuxKernel
{
public:
  static InputParameters validParams() { return VectorAuxKernel::validParams(); }

  ApolloVectorAuxKernel(const InputParameters & parameters) : VectorAuxKernel(parameters) {}

protected:
  // NB: not used.
  virtual RealVectorValue computeValue() override { mooseError("Unused"); }

  /**
   * Returns a *writable* MooseVariable object for a nodal or elemental variable. Use
   * var.setNodalValue(val[, idx]) in both cases (!) to set the solution DOF values. See
   * identically-named method in "Coupleable.h". The reason for this version is that
   * the latter method refuses to work with VectorAuxKernels.
   */
  MooseVariable & writableVariable(const std::string & var_name, unsigned int comp = 0);
};
