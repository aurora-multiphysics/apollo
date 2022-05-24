#include "MFEMParaViewDataCollection.h"

registerMooseObject("ApolloApp", MFEMParaViewDataCollection);

InputParameters
MFEMParaViewDataCollection::validParams()
{
  InputParameters params = MFEMDataCollection::validParams();
  params.addClassDescription(
      "Output for controlling MFEMParaViewDataCollection inherited data.");
  return params;
}

MFEMParaViewDataCollection::MFEMParaViewDataCollection(const InputParameters & parameters)
  : MFEMDataCollection(parameters)
{
 _data_collection = new mfem::ParaViewDataCollection(_file_base.c_str());
}