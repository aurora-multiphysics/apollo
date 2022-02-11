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
  _bc_maps(),
  _mat_map()
{
}

void MFEMProblem::externalSolve(){

    std::cout << "Launching MFEM solve\n\n" << std::endl;
    std::cout << "Proof: " << _input_mesh << std::endl;

    MeshBase &meshy = mesh().getMesh();
    std::cout << meshy.spatial_dimension() << std::endl;


    std::vector<std::string> arguments = {"joule", "-o", "2"};


    double sigma = 2.0*M_PI*10;
    double Tcapacity = 1.0;
    double Tconductivity = 0.01;

    double sigmaAir;
    double TcondAir;
    double TcapAir;

    sigmaAir     = 1.0e-6 * sigma;
    TcondAir     = 1.0e6  * Tconductivity;
    TcapAir      = 1.0    * Tcapacity;

    hephaestus::Material copper("copper", 1);
    copper.setMaterialProperty(std::string("sigma"), sigma);
    copper.setMaterialProperty(std::string("InvTconductivity"), 1.0/Tconductivity);
    copper.setMaterialProperty(std::string("Tcapacity"), Tcapacity);
    copper.setMaterialProperty(std::string("InvTcapacity"), 1.0/Tcapacity);

    hephaestus::Material air("air", 2);
    air.setMaterialProperty(std::string("sigma"), sigmaAir);
    air.setMaterialProperty(std::string("InvTconductivity"), 1.0/TcondAir);
    air.setMaterialProperty(std::string("Tcapacity"), TcapAir);
    air.setMaterialProperty(std::string("InvTcapacity"), 1.0/TcapAir);

    _mat_map.materials.push_back(copper);
    _mat_map.materials.push_back(air);


    hephaestus::Inputs inputs(_problem_type, _input_mesh, _bc_maps, _mat_map);

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

    // _mfem_inputs._mats.push_back(parameters.get<std::vector<SubdomainName>>("block"));

  // _ri_moose_object.parameters()
  }
