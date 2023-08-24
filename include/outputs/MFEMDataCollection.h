#pragma once
#include "FileOutput.h"
#include "mfem.hpp"

/**
 * Class for output information saved in MFEM DataCollections
 */
class MFEMDataCollection : public FileOutput
{
public:
  static InputParameters validParams();

  MFEMDataCollection(const InputParameters & parameters);
  mfem::DataCollection * _data_collection;

protected:
  virtual void output() override{};
};
