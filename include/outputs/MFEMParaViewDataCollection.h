#pragma once
#include "MFEMDataCollection.h"
#include "mfem.hpp"

/**
 * Class for output information saved in MFEM ParaViewDataCollections
 */
class MFEMParaViewDataCollection : public MFEMDataCollection
{
public:
  static InputParameters validParams();

  MFEMParaViewDataCollection(const InputParameters & parameters);
  virtual mfem::ParaViewDataCollection *
  createDataCollection(const std::string & collection_name) override;

protected:
  bool _high_order_output;
  unsigned int _refinements;
};
