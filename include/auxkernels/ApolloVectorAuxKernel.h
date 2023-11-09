#pragma once

#include "AuxKernel.h"

/**
 * Apollo wrapper around VectorAuxKernel.
 *
 * Enables subclasses to call "writableVariable". This is not possible for VectorAuxKernel derived
 * classes due to a MOOSE limitation.
 */
class ApolloVectorAuxKernel : public VectorAuxKernel
{
public:
  static InputParameters validParams() { return VectorAuxKernel::validParams(); }

  ApolloVectorAuxKernel(const InputParameters & parameters) : VectorAuxKernel(parameters) {}

protected:
  // NB: not used.
  virtual RealVectorValue computeValue() override { mooseError("Unused"); }

  // NB: see "writableVariable" method defined in "Coupleable.h".
  MooseVariable & writableVariable(const std::string & var_name, unsigned int comp = 0);
};
