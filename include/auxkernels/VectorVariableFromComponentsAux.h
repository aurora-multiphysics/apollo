#pragma once

#include "VectorVariableToComponentsAux.h"

/**
 * Construct a vector variable from 3 standard variables.
 */
class VectorVariableFromComponentsAux : public VectorVariableToComponentsAux
{
public:
  static InputParameters validParams();

  VectorVariableFromComponentsAux(const InputParameters & parameters);

  void compute() override;

protected:
  /**
   * Override getCoupledVariable. We want non-writable vector variables in this class since we
   * just want to read their values.
   */
  MooseVariable & getCoupledVariable(const std::string & var_name, unsigned int comp) override;
};
