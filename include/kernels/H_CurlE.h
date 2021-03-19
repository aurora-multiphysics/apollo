#pragma once

#include "VectorKernel.h"
#include "MaterialProperty.h"

class H_CurlE : public VectorKernel
{
public:
  static InputParameters validParams();

  H_CurlE(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;

  /// curl of the test function
  const VectorVariableTestCurl & _curl_test;

  /// curl of the shape function
  const VectorVariablePhiCurl & _curl_phi;

  /// Holds the solution curl at the current quadrature points
  const VectorVariableCurl & _curl_u;

  const Function & _x_ffn;
  const Function & _y_ffn;
  const Function & _z_ffn;

  /// The resistivity at the current quadrature point.
  const MaterialProperty<Real> & _rho;
  const MaterialProperty<Real> & _drhodj;
};