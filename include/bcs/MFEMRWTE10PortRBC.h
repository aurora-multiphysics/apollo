#pragma once

#include "MFEMBoundaryCondition.h"
#include "boundary_conditions.hpp"

libMesh::Point PointFromMFEMVector(const mfem::Vector & vec);

class MFEMRWTE10PortRBC : public MFEMBoundaryCondition
{
public:
  static InputParameters validParams();

  MFEMRWTE10PortRBC(const InputParameters & parameters);
  ~MFEMRWTE10PortRBC() override{};

  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}

protected:
  const Real _frequency;
  const RealVectorValue _port_length_vector;
  const RealVectorValue _port_width_vector;
  double _lv[3];
  double _wv[3];
  const bool _input_port;
};
