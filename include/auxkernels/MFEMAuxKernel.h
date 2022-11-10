#pragma once

#include "GeneralUserObject.h"
#include "auxkernels.hpp"
#include "materials.hpp"

class MFEMAuxKernel : public GeneralUserObject
{
public:
  static InputParameters validParams();

  MFEMAuxKernel(const InputParameters & parameters);
  virtual ~MFEMAuxKernel();

  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}

  virtual hephaestus::AuxKernel * getAuxKernel();

  virtual void storeCoefficients(hephaestus::DomainProperties & domain_properties) {}

protected:
  hephaestus::AuxKernel * _auxkernel;
};
