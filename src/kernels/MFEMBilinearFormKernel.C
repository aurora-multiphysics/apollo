#include "MFEMBilinearFormKernel.h"

registerMooseObject("ApolloApp", MFEMBilinearFormKernel);

InputParameters
MFEMBilinearFormKernel::validParams()
{
  InputParameters params = GeneralUserObject::validParams();

  params.registerBase("Kernel");
  params.addParam<std::string>("variable", "Variable on which to apply the kernel");

  return params;
}

MFEMBilinearFormKernel::MFEMBilinearFormKernel(const InputParameters & parameters)
  : GeneralUserObject(parameters)
{
}

MFEMBilinearFormKernel::~MFEMBilinearFormKernel() {}
