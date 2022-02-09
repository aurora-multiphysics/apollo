//* Convert into Action; see INSAction for example.
#include "MFEMInputs.h"

registerMooseObject("ApolloApp", MFEMInputs);

InputParameters
MFEMInputs::validParams()
{
  InputParameters params = MooseObject::validParams();
  params.addRequiredParam<std::string>("input_mesh", "Input mesh for MFEM. "
    "Should be consistent with problem type.");
  params.addRequiredParam<std::string>("problem_type", "Problem type for Joule solver. "
     "Should be 'rod' or 'coil'.");
  return params;
}

MFEMInputs::MFEMInputs(const InputParameters & parameters)
  : MooseObject(parameters),
  _problem_type(getParam<std::string>("problem_type")),
  _input_mesh(getParam<std::string>("input_mesh")),
  _bcs(std::vector<std::vector<BoundaryName>>()),
  _mats(std::vector<std::vector<SubdomainName>>())
{
}

MFEMInputs::~MFEMInputs() {}