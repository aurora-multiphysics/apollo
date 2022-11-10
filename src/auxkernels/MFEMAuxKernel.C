#include "MFEMAuxKernel.h"
#include "Function.h"

registerMooseObject("ApolloApp", MFEMAuxKernel);

InputParameters
MFEMAuxKernel::validParams()
{
  InputParameters params = GeneralUserObject::validParams();

  params.registerBase("AuxKernel");

  return params;
}

MFEMAuxKernel::MFEMAuxKernel(const InputParameters & parameters) : GeneralUserObject(parameters) {}

hephaestus::AuxKernel *
MFEMAuxKernel::getAuxKernel()
{
  return _auxkernel;
}

MFEMAuxKernel::~MFEMAuxKernel() {}
