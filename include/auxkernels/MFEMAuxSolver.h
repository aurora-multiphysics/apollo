#pragma once

#include "GeneralUserObject.h"
#include "auxsolvers.hpp"
#include "gridfunctions.hpp"

class MFEMAuxSolver : public GeneralUserObject
{
public:
  static InputParameters validParams();

  MFEMAuxSolver(const InputParameters & parameters);
  virtual ~MFEMAuxSolver();

  virtual void execute() override {}
  virtual void initialize() override {}
  virtual void finalize() override {}

  virtual hephaestus::AuxSolver * getAuxSolver();

  virtual void storeCoefficients(hephaestus::Coefficients & coefficients) {}

protected:
  hephaestus::AuxSolver * _auxsolver;
};
