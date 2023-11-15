#include "MFEMVisItDataCollection.h"

registerMooseObject("ApolloApp", MFEMVisItDataCollection);

InputParameters
MFEMVisItDataCollection::validParams()
{
  InputParameters params = MFEMDataCollection::validParams();
  params.addClassDescription("Output for controlling MFEMVisItDataCollection inherited data.");
  params.addParam<unsigned int>("refinements",
                                0,
                                "Number of uniform refinements for oversampling "
                                "(refinement levels beyond any uniform "
                                "refinements)");
  return params;
}

MFEMVisItDataCollection::MFEMVisItDataCollection(const InputParameters & parameters)
  : MFEMDataCollection(parameters), _refinements(getParam<unsigned int>("refinements"))
{
}

mfem::VisItDataCollection *
MFEMVisItDataCollection::createDataCollection(const std::string & collection_name)
{
  mfem::VisItDataCollection * visit_dc(
      new mfem::VisItDataCollection(_file_base.c_str() + collection_name));
  visit_dc->SetLevelsOfDetail(_refinements + 1);
  return visit_dc;
}