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
    std::cout << "BCs: " << _mfem_inputs._bcs[0][0] << std::endl;

    MeshBase &meshy = mesh().getMesh();
    std::cout << meshy.spatial_dimension() << std::endl;


    std::vector<std::string> arguments = {"joule", "-m", _input_mesh, "-o", "2", "-p", _problem_type};

    std::vector<BCMap> bc_maps({
        BCMap(std::string("curl_bc"), Array<int>({1,2,3})),
        BCMap(std::string("thermal_bc"), Array<int>({1,2})),
        BCMap(std::string("poisson_bc"), Array<int>({1,2})),
    });
    Inputs inputs(bc_maps);

    std::vector<char*> argv;
    for (const auto& arg : arguments)
        argv.push_back((char*)arg.data());
    argv.push_back(nullptr);

    joule_solve(argv.size() - 1, argv.data(), inputs);
}

  void MFEMProblem::addBoundaryCondition(const std::string & bc_name,
                            const std::string & name,
                            InputParameters & parameters)
  {
    _mfem_inputs._bcs.push_back(parameters.get<std::vector<BoundaryName>>("boundary"));
  }


  void MFEMProblem::addMaterial(const std::string & kernel_name,
                   const std::string & name,
                   InputParameters & parameters)
  {
    _mfem_inputs._mats.push_back(parameters.get<std::vector<SubdomainName>>("block"));
  }
