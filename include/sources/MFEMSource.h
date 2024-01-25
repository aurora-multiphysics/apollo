#pragma once

#include "GeneralUserObject.h"
#include "Function.h"
#include "MFEMFESpace.h"
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

  virtual std::shared_ptr<hephaestus::Source> getSource() { return _source; }

  virtual void storeCoefficients(hephaestus::Coefficients & coefficients);

protected:
  std::vector<SubdomainID> blocks;
  std::shared_ptr<hephaestus::Source> _source{nullptr};
};
