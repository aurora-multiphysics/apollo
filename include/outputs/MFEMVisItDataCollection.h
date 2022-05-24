#pragma once
#include "MFEMDataCollection.h"
#include "mfem.hpp"

/**
 * Class for output information saved in MFEM DataCollections
 */
class MFEMVisItDataCollection : public MFEMDataCollection
{
public:
  static InputParameters validParams();

  MFEMVisItDataCollection(const InputParameters & parameters);

protected:
  virtual void output(const ExecFlagType & type) override {};
};
