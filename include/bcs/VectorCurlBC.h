#pragma once

#include "VectorIntegratedBC.h"

class VectorCurlBC : public VectorIntegratedBC
{
public:
  static InputParameters validParams();

  VectorCurlBC(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();

  const Function & _curl_value;
};
