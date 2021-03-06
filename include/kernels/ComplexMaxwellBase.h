//*
#pragma once

#include "MaxwellBase.h"
#include "MaterialProperty.h"

class ComplexMaxwellBase : public VectorKernel
{
public:
  static InputParameters validParams();

  ComplexMaxwellBase(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;

  virtual Real curlCurlTerm();
  virtual Real gaugePenaltyTerm();
  virtual Real dCurlCurlDU();
  virtual Real dGaugePenaltyDU();

  virtual Real coupledCurlCurlTerm();
  // virtual Real coupledGaugePenaltyTerm();
  virtual Real dCoupledCurlCurlDU();

  /// curl of the shape function
  const VectorVariablePhiCurl & _curl_phi;

  /// curl of the test function
  const VectorVariableTestCurl & _curl_test;

  /// Holds the solution curl at the current quadrature points
  const VectorVariableCurl & _curl_u;

  const unsigned int _v_id;
  // Holds the solution curl at the current quadrature points
  const VectorVariableValue & _v;
  const VectorVariableCurl & _curl_v;
};
