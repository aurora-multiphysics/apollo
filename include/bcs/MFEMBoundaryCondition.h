#pragma once

#include "GeneralUserObject.h"
#include "boundary_conditions.hpp"
#include "materials.hpp"

libMesh::Point PointFromMFEMVector(const mfem::Vector & vec);

class MFEMBoundaryCondition : public GeneralUserObject
{
public:
  static InputParameters validParams();

  MFEMBoundaryCondition(const InputParameters & parameters);
  virtual ~MFEMBoundaryCondition();

  virtual hephaestus::BoundaryCondition * getBC();
  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}
  virtual void storeCoefficients(hephaestus::DomainProperties & domain_properties) {}

protected:
  std::vector<BoundaryName> _boundary_names;
  mfem::Array<int> bdr_attr;
  hephaestus::BoundaryCondition * _boundary_condition;
};
