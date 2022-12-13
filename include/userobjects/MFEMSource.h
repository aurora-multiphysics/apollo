#pragma once

#include "GeneralUserObject.h"
#include "sources.hpp"

libMesh::Point PointFromMFEMVector(const mfem::Vector & vec);

class MFEMSource : public GeneralUserObject
{
public:
  static InputParameters validParams();

  MFEMSource(const InputParameters & parameters);
  virtual ~MFEMSource();

  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}

  virtual hephaestus::Source * getSource();
  virtual void storeCoefficients(hephaestus::DomainProperties & domain_properties);

protected:
  std::vector<SubdomainID> blocks;
  const Function & _func;
  mfem::VectorFunctionCoefficient _vec_function_coef;
  mfem::Array<mfem::VectorCoefficient *> sourcecoefs;
  mfem::Array<int> coilsegments;

  mfem::PWVectorCoefficient * _restricted_coef;

  hephaestus::Source * _source;  
};
