#include "MFEMADIOS2DataCollection.h"

registerMooseObject("ApolloApp", MFEMADIOS2DataCollection);

InputParameters
MFEMADIOS2DataCollection::validParams()
{
  InputParameters params = MFEMDataCollection::validParams();
  params.addClassDescription("Output for controlling MFEMADIOS2DataCollection inherited data.");
  params.addParam<unsigned int>("refinements",
                                0,
                                "Number of uniform refinements for oversampling "
                                "(refinement levels beyond any uniform "
                                "refinements)");
  return params;
}

MFEMADIOS2DataCollection::MFEMADIOS2DataCollection(const InputParameters & parameters)
  : MFEMDataCollection(parameters), _refinements(getParam<unsigned int>("refinements"))
{
}

std::shared_ptr<mfem::DataCollection>
MFEMADIOS2DataCollection::createDataCollection(const std::string & collection_name) const
{
  auto adios_dc = std::make_shared<mfem::ADIOS2DataCollection>(
      MPI_COMM_WORLD, _file_base.c_str() + collection_name + ".bp");

  adios_dc->SetPrecision(9);
  adios_dc->SetLevelsOfDetail(_refinements + 1);

  return adios_dc;
}