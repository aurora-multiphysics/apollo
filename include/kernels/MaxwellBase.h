//*
#pragma once

#include "VectorTimeKernel.h"
#include "MaterialProperty.h"

class MaxwellBase : public VectorTimeKernel
{
public:
  static InputParameters validParams();

  MaxwellBase(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;

  virtual Real curlCurlTerm();
  virtual Real gaugePenaltyTerm();
  virtual Real dCurlCurlDU();
  virtual Real dGaugePenaltyDU();
  virtual Real steadyStateTerm();
  virtual Real dSteadyStateTermDU();
  virtual Real firstOrderTimeDerivTerm();
  virtual Real dFirstOrderTimeDerivDU();
  virtual Real secondOrderTimeDerivTerm();
  virtual Real dSecondOrderTimeDerivDU();
  /// curl of the shape function
  const VectorVariablePhiCurl & _curl_phi;

  /// curl of the test function
  const VectorVariableTestCurl & _curl_test;

  /// Holds the solution curl at the current quadrature points
  const VectorVariableCurl & _curl_u;

  /// Time derivative of u
  const VectorVariableValue & _u_dotdot;

  /// Derivative of u_dot and u_dotdot with respect to u
  const VariableValue & _du_dotdot_du;

  bool _quasistationary;
  bool _gauge_penalty;

};
