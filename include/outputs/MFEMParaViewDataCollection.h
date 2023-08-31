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

protected:
  mfem::ParaViewDataCollection _pv_dc;
  bool _high_order_output;
  unsigned int _refinements;
};
