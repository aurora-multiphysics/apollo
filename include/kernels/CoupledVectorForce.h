#pragma once

#include "VectorKernel.h"

// Forward Declaration
class CoupledVectorForce;

template <>
InputParameters validParams<CoupledVectorForce>();

/**
 * Simple class to demonstrate off diagonal Jacobian contributions.
 */
class CoupledVectorForce : public VectorKernel
{
public:
  static InputParameters validParams();

  CoupledVectorForce(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;

  virtual Real computeQpJacobian() override;

  virtual Real computeQpOffDiagJacobian(unsigned int jvar) override;

private:
  unsigned int _v_var;
  const VectorVariableValue & _v;
  Real _coef;
};
