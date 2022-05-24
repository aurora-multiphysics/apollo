#pragma once
#include "MFEMDataCollection.h"
#include "mfem.hpp"

/**
 * Class for output information saved in MFEM DataCollections
 */
class MFEMParaViewDataCollection : public MFEMDataCollection
{
public:
  static InputParameters validParams();

  MFEMParaViewDataCollection(const InputParameters & parameters);

protected:
  virtual void output(const ExecFlagType & type) override {};
};
