#pragma once

#include "VectorIntegratedBC.h"

class AVCurrentFlowBC : public VectorIntegratedBC
{
public:
  static InputParameters validParams();

  AVCurrentFlowBC(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();

  const Function & _curl_value;
};
