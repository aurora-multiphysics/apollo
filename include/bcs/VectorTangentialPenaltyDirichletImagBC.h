#pragma once

#include "VectorIntegratedBC.h"

class VectorTangentialPenaltyDirichletImagBC : public VectorIntegratedBC
{
public:
  static InputParameters validParams();

  VectorTangentialPenaltyDirichletImagBC(const InputParameters & parameters);

protected:
  Real _penalty;
  const VectorVariableValue & _v;
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar) override;
  // Holds the solution curl at the current quadrature points
  const unsigned int _v_id;
  const Function * const _function;
  const Function & _function_x;
  const Function & _function_y;
  const Function & _function_z;

};
