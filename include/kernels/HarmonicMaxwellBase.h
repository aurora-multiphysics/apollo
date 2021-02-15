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

#include "MaxwellBase.h"
#include "MaterialProperty.h"

class HarmonicMaxwellBase : public MaxwellBase
{
public:
  static InputParameters validParams();

  HarmonicMaxwellBase(const InputParameters & parameters);
  
protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;

  virtual Real coupledCurlCurlTerm();
  // virtual Real coupledGaugePenaltyTerm();
  virtual Real dCoupledCurlCurlDU();
  // virtual Real dCoupledGaugePenaltyDU();
  // virtual Real coupledFirstOrderTimeDerivTerm();
  // virtual Real dCoupledFirstOrderTimeDerivDU();
  // virtual Real coupledSecondOrderTimeDerivTerm();
  // virtual Real dCoupledSecondOrderTimeDerivDU();
  const unsigned int _v_id;
  // Holds the solution curl at the current quadrature points
  const VectorVariableValue & _v;
  const VectorVariableCurl & _curl_v;
};
