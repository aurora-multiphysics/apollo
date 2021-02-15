//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
//*
#pragma once

#include "HarmonicMaxwellBase.h"
#include "MaterialProperty.h"

class HarmonicMaxwellImag : public HarmonicMaxwellBase
{
public:
  static InputParameters validParams();

  HarmonicMaxwellImag(const InputParameters & parameters);
  
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
