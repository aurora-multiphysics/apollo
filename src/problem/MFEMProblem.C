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
  _problem_type(getParam<std::string>("problem_type")),
  _input_mesh(getParam<std::string>("input_mesh")),
  _bc_maps()
{
}

void MFEMProblem::externalSolve(){

    std::cout << "Launching MFEM solve\n\n" << std::endl;
    std::cout << "Proof: " << _input_mesh << std::endl;

    MeshBase &meshy = mesh().getMesh();
    std::cout << meshy.spatial_dimension() << std::endl;


    std::vector<std::string> arguments = {"joule", "-o", "2"};

    Inputs inputs(_problem_type, _input_mesh, _bc_maps);

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
    std::vector<BoundaryName> boundary = parameters.get<std::vector<BoundaryName>>("boundary");
    mfem::Array<int> bdr_attr(boundary.size());

    for (unsigned int i = 0; i < boundary.size(); ++i)
    {
      bdr_attr[i] = std::stoi(boundary[i]);
    }
    BCMap bc(name, bdr_attr);
    _bc_maps.push_back(bc);
  }


  void MFEMProblem::addMaterial(const std::string & kernel_name,
                   const std::string & name,
                   InputParameters & parameters)
  {

    // _mfem_inputs._mats.push_back(parameters.get<std::vector<SubdomainName>>("block"));

  // _ri_moose_object.parameters()
  }
