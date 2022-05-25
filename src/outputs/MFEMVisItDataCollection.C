#include "MFEMVisItDataCollection.h"

registerMooseObject("ApolloApp", MFEMVisItDataCollection);

InputParameters
MFEMVisItDataCollection::validParams()
{
  InputParameters params = MFEMDataCollection::validParams();
  params.addClassDescription(
      "Output for controlling MFEMVisItDataCollection inherited data.");
  params.addParam<unsigned int>("refinements",
                                0,
                                "Number of uniform refinements for oversampling "
                                "(refinement levels beyond any uniform "
                                "refinements)");
  return params;
}

MFEMVisItDataCollection::MFEMVisItDataCollection(const InputParameters & parameters)
  : MFEMDataCollection(parameters),
    _visit_dc(_file_base.c_str()),
    _refinements(getParam<unsigned int>("refinements"))
{
  _visit_dc.SetLevelsOfDetail(_refinements + 1);
 _data_collection = &_visit_dc;
}