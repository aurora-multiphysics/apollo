#pragma once

#include "MaxwellBase.h"
#include "MaterialProperty.h"

class Maxwell_E : public MaxwellBase
{
public:
  static InputParameters validParams();

  Maxwell_E(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;

  const MaterialProperty<Real> & _rho;
  const MaterialProperty<Real> & _mu;
};
