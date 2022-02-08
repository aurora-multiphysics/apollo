#include "MFEMProblem.h"
#include "SystemBase.h"

#include "hephaestus_joule.hpp"


registerMooseObject("ApolloApp", MFEMProblem);


InputParameters MFEMProblem::validParams()
{
  InputParameters params = ExternalProblem::validParams();
  params.addParam<std::string>("input_mesh", "Input mesh for MFEM. "
  "Should be consistent with problem type.");
  params.addParam<std::string>("problem_type", "Problem type for Joule solver. "
  "Should be 'rod' or 'coil'.");
  return params;
}

MFEMProblem::MFEMProblem(const InputParameters & params)
  : ExternalProblem(params),
  _mfem_inputs(params)
{
}

void MFEMProblem::externalSolve(){
    std::string _problem_type = _mfem_inputs._problem_type;
    std::string _input_mesh = _mfem_inputs._input_mesh;

    std::cout << "Launching MFEM solve\n\n" << std::endl;
    std::cout << "Proof: " << _input_mesh << std::endl;
    std::cout << "BCs: " << _mfem_inputs._bcs[2] << std::endl;

    MeshBase &meshy = mesh().getMesh();
    std::cout << meshy.spatial_dimension() << std::endl;

    std::vector<std::string> arguments = {"joule", "-m", _input_mesh, "-o", "2", "-p", _problem_type};

    std::vector<char*> argv;
    for (const auto& arg : arguments)
        argv.push_back((char*)arg.data());
    argv.push_back(nullptr);

    joule_solve(argv.size() - 1, argv.data());
}

  void MFEMProblem::addBoundaryCondition(const std::string & bc_name,
                            const std::string & name,
                            InputParameters & parameters)
  {
    _mfem_inputs._bcs = parameters.get<std::vector<BoundaryName>>("boundary");
  }

