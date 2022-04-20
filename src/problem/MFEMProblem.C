#include "MFEMProblem.h"
#include "SystemBase.h"
#include "Transient.h"
#include "hephaestus.hpp"

registerMooseObject("ApolloApp", MFEMProblem);

InputParameters
MFEMProblem::validParams()
{
  InputParameters params = ExternalProblem::validParams();
  params.addParam<std::string>("input_mesh", "Input mesh for MFEM.");
  params.addParam<std::string>("formulation", "Name of EM formulation to use in MFEM.");
  params.addParam<int>("order", "Order of the FE variables for MFEM.");
  params.addParam<double>("dt", "Time step");
  params.addParam<double>("end_time", "Time at which to end transient simulation.");

  return params;
}

MFEMProblem::MFEMProblem(const InputParameters & params)
  : ExternalProblem(params),
    _input_mesh(_mesh.parameters().get<MeshFileName>("file")),
    _formulation(getParam<std::string>("formulation")),
    _order(getParam<int>("order")),
    _bc_maps(),
    _mat_map(),
    _executioner(std::string("Transient"), getParam<double>("dt"), getParam<double>("end_time"))
{
}

void
MFEMProblem::externalSolve()
{
  hephaestus::Inputs inputs(_input_mesh, _formulation, _order, _bc_maps, _mat_map, _executioner);

  std::vector<char *> argv;
  std::cout << "Launching MFEM solve\n\n" << std::endl;
  run_hephaestus(argv.size() - 1, argv.data(), inputs);
}

libMesh::Point
PointFromMFEMVector(const mfem::Vector & vec)
{
  return libMesh::Point(vec.Elem(0), vec.Elem(1), vec.Elem(2));
}

void
MFEMProblem::addBoundaryCondition(const std::string & bc_name,
                                  const std::string & name,
                                  InputParameters & parameters)
{
  std::vector<BoundaryName> boundaries = parameters.get<std::vector<BoundaryName>>("boundary");
  mfem::Array<int> bdr_attr(boundaries.size());

  for (unsigned int i = 0; i < boundaries.size(); ++i)
  {
    bdr_attr[i] = std::stoi(boundaries[i]);
  }

  if (parameters.isParamValid("function"))
  {
    const FunctionName & function_name(parameters.get<FunctionName>("function"));
    const Function & _func(getFunction(function_name));

    _bc_maps[name] = new hephaestus::FunctionDirichletBC(
        name, bdr_attr, new mfem::FunctionCoefficient([&](const mfem::Vector & p, double t) {
          return _func.value(t, PointFromMFEMVector(p));
        }));
  }
  else
  {
    _bc_maps[name] = new hephaestus::BoundaryCondition(name, bdr_attr);
  }
}

void
MFEMProblem::addMaterial(const std::string & kernel_name,
                         const std::string & name,
                         InputParameters & parameters)
{
  FEProblemBase::addUserObject(kernel_name, name, parameters);
  const MFEMMaterial & mfem_material(getUserObject<MFEMMaterial>(name));

  for (unsigned int bid = 0; bid < mfem_material.blocks.size(); ++bid)
  {
    int block = std::stoi(mfem_material.blocks[bid]);
    hephaestus::Subdomain mfem_subdomain(name, block);
    mfem_subdomain.property_map = mfem_material.scalar_property_map;
    _mat_map.subdomains.push_back(mfem_subdomain);
  }
}
