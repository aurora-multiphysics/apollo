#pragma once

#include "AuxKernel.h"

/**
 * Apollo wrapper around VectorAuxKernel.
 *
 * Enables subclasses to call "writableVariable". This is not possible for VectorAuxKernel derived
 * classes due to a MOOSE limitation.
 */
class WritableVectorAuxKernel : public VectorAuxKernel
{
public:
  static InputParameters validParams() { return VectorAuxKernel::validParams(); }

  WritableVectorAuxKernel(const InputParameters & parameters);

protected:
  // NB: not used.
  virtual RealVectorValue computeValue() override { mooseError("Unused"); }

  const Order _vector_order;
  const FEFamily _vector_family;

  /**
   * This method is based on "writableVariable" defined in "Coupleable.h". Due to a limitation of
   * MOOSE it is not possible to call this from a VectorAuxKernel class.
   *
   * NB: only a single AuxKernel can obtain a writable reference to a variable. This becomes a
   * problem for transfers where we need to push and pull the same variable using the auxkernels
   * inheriting from this class.
   *
   * For VectorVariableToComponentsAux, we require writable references.
   */
  MooseVariable & getWritableCoupledVariable(const std::string & var_name, unsigned int comp = 0);

  /**
   * Returns a const-qualified reference to the coupled variable. This is required for
   * VectorVariableFromComponentsAux since we don't need to update the values of the component
   * variables.
   */
  const MooseVariable & getNonWritableCoupledVariable(const std::string & var_name,
                                                      unsigned int comp = 0);

  /**
   * Check component variables are consistent with the vector variable.
   */
  void checkVectorVariable() const;
  void checkVectorComponent(const MooseVariable & component_variable) const;
  void checkVectorComponents(const MooseVariable & component_x,
                             const MooseVariable & component_y,
                             const MooseVariable & component_z) const;
};
