#include "MFEMDataCollection.h"

registerMooseObject("ApolloApp", MFEMDataCollection);

InputParameters
MFEMDataCollection::validParams()
{
  InputParameters params = FileOutput::validParams();
  params.addClassDescription(
      "Output for controlling MFEMDataCollection inherited data.");
  return params;
}

MFEMDataCollection::MFEMDataCollection(const InputParameters & parameters)
  : FileOutput(parameters)
{
 _data_collection = new mfem::DataCollection(_file_base.c_str());
}