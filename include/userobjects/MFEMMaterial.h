#pragma once

#include "GeneralUserObject.h"
#include "materials.hpp"

class MFEMMaterial : public GeneralUserObject
{
public:
  static InputParameters validParams();

  MFEMMaterial(const InputParameters & parameters);
  virtual ~MFEMMaterial();

  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}

  std::map<std::string, mfem::Coefficient *> scalar_property_map;
  std::vector<SubdomainName> blocks;
};
