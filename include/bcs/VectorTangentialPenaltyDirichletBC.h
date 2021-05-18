#pragma once

#include "VectorIntegratedBC.h"

class VectorTangentialPenaltyDirichletBC : public VectorIntegratedBC
{
public:
  static InputParameters validParams();

  VectorTangentialPenaltyDirichletBC(const InputParameters & parameters);

protected:
  Real _penalty;
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
  // Holds the solution curl at the current quadrature points
  const Function * const _function;
  const Function & _function_x;
  const Function & _function_y;
  const Function & _function_z;

};
