#pragma once

#include "GeneralUserObject.h"
#include "materials.hpp"

class MFEMCoefficient : public GeneralUserObject
{
public:
  static InputParameters validParams();

  MFEMCoefficient(const InputParameters & parameters);
  virtual ~MFEMCoefficient();

  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}
};
