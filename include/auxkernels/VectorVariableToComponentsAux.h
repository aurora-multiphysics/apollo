#pragma once

#include "WritableVectorAuxKernel.h"

/**
 * Set 3 standard variables from a vector variable.
 */
class VectorVariableToComponentsAux : public WritableVectorAuxKernel
{
public:
  static InputParameters validParams();

  VectorVariableToComponentsAux(const InputParameters & parameters);

  void compute() override;

protected:
  MooseVariable & _component_x;
  MooseVariable & _component_y;
  MooseVariable & _component_z;

  /**
   * Virtual function that can be overriden in VectorVariableFromComponentsAux. We want writable
   * coupled variables in this class since we're updating their values.
   */
  virtual MooseVariable & getCoupledVariable(const std::string & var_name, unsigned int comp);

  void checkVectorComponents() const;
};
