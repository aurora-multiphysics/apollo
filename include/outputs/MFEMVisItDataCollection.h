#pragma once
#include "MFEMDataCollection.h"
#include "mfem.hpp"

/**
 * Class for output information saved in MFEM VisItDataCollections
 */
class MFEMVisItDataCollection : public MFEMDataCollection
{
public:
  static InputParameters validParams();

  MFEMVisItDataCollection(const InputParameters & parameters);

protected:
  mfem::VisItDataCollection _visit_dc;
  bool _high_order_output;
  unsigned int _refinements;
};
