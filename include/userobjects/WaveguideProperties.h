//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "GeneralUserObject.h"

class WaveguideProperties : public GeneralUserObject
{
public:
  static InputParameters validParams();

  WaveguideProperties(const InputParameters & parameters);
  virtual ~WaveguideProperties();

  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}

  virtual Real getImagPropagationConstant() const;
  virtual RealVectorValue getImagRWTE10(Real t, const Point & p) const;
  virtual RealVectorValue getRealRWTE10(Real t, const Point & p) const;

  /// EM constants
  static const Real _mu0;
  static const Real _epsilon0;

  const Real _a;
  const Real _b;
  const Real _omega;
protected:


};
