//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
//*
#pragma once

#include "MaxwellBase.h"
#include "MaterialProperty.h"

class EddyTOFaraday : public MaxwellBase
{
public:
  static InputParameters validParams();

  EddyTOFaraday(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
  virtual Real computeQpOffDiagJacobian(unsigned int jvar) override;

  const VariableGradient & _grad_v;
  const unsigned _v_id;
  MooseVariable & _v_var;
  const VariablePhiGradient & _standard_grad_phi;

  const MaterialProperty<Real> & _rho;
  const MaterialProperty<Real> & _mu;

  /// The resistivity at the current quadrature point.
  // const MaterialProperty<Real> & _permeability;
  // const MaterialProperty<Real> & _resistivity;
  // const MaterialProperty<Real> & _drhodj;
};
