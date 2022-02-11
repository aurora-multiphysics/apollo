#include "MFEMProblem.h"
#include "SystemBase.h"

#include "hephaestus_joule.hpp"


registerMooseObject("ApolloApp", MFEMProblem);


InputParameters MFEMProblem::validParams()
{
  InputParameters params = ExternalProblem::validParams();
  params.addParam<std::string>("input_mesh", "Input mesh for MFEM.");
  return params;
}

MFEMProblem::MFEMProblem(const InputParameters & params)
  : ExternalProblem(params),
  _input_mesh(getParam<std::string>("input_mesh")),
  _bc_maps(),
  _mat_map()
{
}

void MFEMProblem::externalSolve(){

    std::cout << "Launching MFEM solve\n\n" << std::endl;
    std::cout << "Proof: " << _input_mesh << std::endl;

    hephaestus::Inputs inputs(_input_mesh, _bc_maps, _mat_map);

    std::vector<std::string> arguments = {"joule", "-o", "2"};

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
    hephaestus::BoundaryCondition bc(name, bdr_attr);
    _bc_maps.setBC(name, bc);
  }


  void MFEMProblem::addMaterial(const std::string & kernel_name,
                   const std::string & name,
                   InputParameters & parameters)
  {

    std::vector<SubdomainName> blocks = parameters.get<std::vector<SubdomainName>>("block");
    std::vector<std::string> property_names = parameters.get<std::vector<std::string>>("prop_names");
    std::vector<double> property_values = parameters.get<std::vector<double>>("prop_values");

    for (unsigned int bid = 0; bid < blocks.size(); ++bid)
    {
      int block = std::stoi(blocks[bid]);
      hephaestus::Material mat(name, block);
      for (unsigned int pid = 0; pid < property_names.size(); ++pid)
      {
        mat.setMaterialProperty(property_names[pid], property_values[pid]);
      }
      _mat_map.materials.push_back(mat);
    }

  }
