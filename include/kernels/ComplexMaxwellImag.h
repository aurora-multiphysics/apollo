//*
#pragma once

#include "ComplexMaxwellBase.h"
#include "MaterialProperty.h"
#include "WaveguideProperties.h"

class ComplexMaxwellImag : public ComplexMaxwellBase
{
public:
  static InputParameters validParams();

  ComplexMaxwellImag(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
  virtual Real computeQpOffDiagJacobian(unsigned int jvar) override;

  const Real _omega;
  const MaterialProperty<Real> & _sigma_re;
  const MaterialProperty<Real> & _sigma_im;
  const MaterialProperty<Real> & _epsilon_re;
  const MaterialProperty<Real> & _epsilon_im;
  const MaterialProperty<Real> & _nu_re;
  const MaterialProperty<Real> & _nu_im;
};
