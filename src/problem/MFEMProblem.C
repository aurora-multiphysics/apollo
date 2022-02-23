#include "MFEMProblem.h"
#include "SystemBase.h"

#include "hephaestus.hpp"


registerMooseObject("ApolloApp", MFEMProblem);


InputParameters MFEMProblem::validParams()
{
  InputParameters params = ExternalProblem::validParams();
  params.addParam<std::string>("input_mesh", "Input mesh for MFEM.");
  params.addParam<std::string>("formulation", "Name of EM formulation to use in MFEM.");
  params.addParam<int>("order", "Order of the FE variables for MFEM.");
  return params;
}


MFEMProblem::MFEMProblem(const InputParameters & params)
  : ExternalProblem(params),
  _input_mesh(getParam<std::string>("input_mesh")),
  _formulation(getParam<std::string>("formulation")),
  _order(getParam<int>("order")),
  _bc_maps(),
  _mat_map()
{
}


void MFEMProblem::externalSolve(){
    hephaestus::Inputs inputs(_input_mesh, _formulation, _order, _bc_maps, _mat_map);

    std::vector<char*> argv;
    std::cout << "Launching MFEM solve\n\n" << std::endl;
    run_hephaestus(argv.size() - 1, argv.data(), inputs);
}


libMesh::Point PointFromMFEMVector(const mfem::Vector &vec)
{
  return libMesh::Point(vec.Elem(0),vec.Elem(1),vec.Elem(2));
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

  if (parameters.isParamValid("function"))
  {
    const FunctionName & function_name(parameters.get<FunctionName>("function"));
    const Function & _func(getFunction(function_name));
    bc.scalar_func = std::bind(&Function::value, &_func, std::placeholders::_2,  std::bind(PointFromMFEMVector, std::placeholders::_1));
  }

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
