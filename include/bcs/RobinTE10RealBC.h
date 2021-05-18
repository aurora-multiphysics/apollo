#pragma once

#include "VectorIntegratedBC.h"
#include "WaveguideProperties.h"
class RobinTE10RealBC : public VectorIntegratedBC
{
public:
  static InputParameters validParams();

  RobinTE10RealBC(const InputParameters & parameters);

protected:
  Real _penalty;
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
  virtual Real computeQpOffDiagJacobian(unsigned int jvar) override;
  // Holds the solution curl at the current quadrature points
  const unsigned int _v_id;
  const VectorVariableValue & _v;
  const WaveguideProperties & _wp;
  bool _input_port;
};
