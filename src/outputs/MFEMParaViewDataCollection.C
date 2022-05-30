#include "MFEMParaViewDataCollection.h"

registerMooseObject("ApolloApp", MFEMParaViewDataCollection);

InputParameters
MFEMParaViewDataCollection::validParams()
{
  InputParameters params = MFEMDataCollection::validParams();
  params.addClassDescription("Output for controlling MFEMParaViewDataCollection inherited data.");
  params.addParam<unsigned int>("refinements",
                                0,
                                "Number of uniform refinements for oversampling "
                                "(refinement levels beyond any uniform "
                                "refinements)");
  params.addParam<bool>("high_order_output",
                        false,
                        "Sets whether or not to output the data as "
                        "high-order elements (false by default)."
                        "Reading high-order data requires ParaView"
                        "5.5 or later.");
  return params;
}

MFEMParaViewDataCollection::MFEMParaViewDataCollection(const InputParameters & parameters)
  : MFEMDataCollection(parameters),
    _pv_dc(_file_base.c_str()),
    _high_order_output(getParam<bool>("high_order_output")),
    _refinements(getParam<unsigned int>("refinements"))
{
  _pv_dc.SetHighOrderOutput(_high_order_output);
  _pv_dc.SetLevelsOfDetail(_refinements + 1);
  _data_collection = &_pv_dc;
}
