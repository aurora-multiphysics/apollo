//* Solves:
//* curl(xi * curl u) = 0
//*
//* in weak form:
//* (xi * curl u, curl v) = 0

//* For T-phi formulation, u = T, p = omega, xi = rho, eta = mu
//* and H = T0 + T - grad phi = u + grad p

//* For A-V formulation, u = A, p = int(V dt), xi = mu^-1, eta = sigma
//* B = curl A

#pragma once

#include "VectorKernel.h"
#include "MaterialProperty.h"

class CurlCurl : public VectorKernel
{
public:
  static InputParameters validParams();

  CurlCurl(const InputParameters & parameters);
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
protected:


  /// curl of the shape function
  const VectorVariablePhiCurl & _curl_phi;

  /// curl of the test function
  const VectorVariableTestCurl & _curl_test;

  /// Holds the solution curl at the current quadrature points
  const VectorVariableCurl & _curl_u;
};
