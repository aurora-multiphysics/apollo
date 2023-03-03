#include "MFEMDiffusionKernel.h"

registerMooseObject("ApolloApp", MFEMDiffusionKernel);

InputParameters
MFEMDiffusionKernel::validParams()
{
  InputParameters params = MFEMBilinearFormKernel::validParams();
  params.addClassDescription(
      "The Laplacian operator ($-k\\nabla \\cdot \\nabla u$), with the weak "
      "form of $ (k\\nabla \\phi_i, \\nabla u_h), to be added to an MFEM problem");

  params.addParam<std::string>("coefficient",
                               "Name of MFEM coefficient k to multiply the Laplacian by");

  return params;
}

MFEMDiffusionKernel::MFEMDiffusionKernel(const InputParameters & parameters)
  : MFEMBilinearFormKernel(parameters),
    kernel_params({{"VariableName", getParam<std::string>("variable")},
                   {"CoefficientName", getParam<std::string>("coefficient")}}),
    kernel(kernel_params)
{
}

hephaestus::Kernel<mfem::ParBilinearForm> *
MFEMDiffusionKernel::getKernel()
{
  return &kernel;
}

MFEMDiffusionKernel::~MFEMDiffusionKernel() {}
