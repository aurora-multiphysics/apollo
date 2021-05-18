//*
#pragma once

#include "TimeKernel.h"
#include "MaterialProperty.h"

class EddyTOGauss : public TimeKernel
{
public:
  static InputParameters validParams();

  EddyTOGauss(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
  virtual Real computeQpOffDiagJacobian(unsigned int jvar) override;
  const VariableGradient & _grad_u_dot;
  const VariableValue & _du_dot_du;
  const unsigned int _v_id;
  // const VectorVariableValue & _v;
  // const VectorVariableGradient & _grad_v;
  const VectorVariableValue & _v_dot;
  const VariableValue & _dv_dot_dv;
  VectorMooseVariable & _v_var;
  const VectorVariablePhiValue & _vector_phi;
  const MaterialProperty<Real> & _mu;
  Real _sgn;
};
