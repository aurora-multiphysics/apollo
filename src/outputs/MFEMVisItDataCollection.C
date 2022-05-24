#include "MFEMVisItDataCollection.h"

registerMooseObject("ApolloApp", MFEMVisItDataCollection);

InputParameters
MFEMVisItDataCollection::validParams()
{
  InputParameters params = MFEMDataCollection::validParams();
  params.addClassDescription(
      "Output for controlling MFEMVisItDataCollection inherited data.");
  return params;
}

MFEMVisItDataCollection::MFEMVisItDataCollection(const InputParameters & parameters)
  : MFEMDataCollection(parameters)
{
 _data_collection = new mfem::VisItDataCollection(_file_base.c_str());
}