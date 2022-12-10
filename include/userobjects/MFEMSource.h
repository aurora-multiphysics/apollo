#pragma once

#include "GeneralUserObject.h"
#include "sources.hpp"

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
  hephaestus::Source * _source;
  mfem::VectorFunctionCoefficient * JSrcCoef;
  mfem::PWVectorCoefficient * JSrcRestricted;
};
