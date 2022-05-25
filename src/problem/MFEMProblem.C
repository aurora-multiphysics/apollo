#include "MFEMProblem.h"
#include "SystemBase.h"
#include "Transient.h"
#include "MFEMFunctionDirichletBC.h"
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
    _executioner(std::string("Transient"), getParam<double>("dt"), getParam<double>("end_time")),
    _outputs()
{
}

void
MFEMProblem::externalSolve()
{
  mfem::Mesh mfem_mesh(_input_mesh.c_str(),1,1);
  std::vector<OutputName> mfem_data_collections = _app.getOutputWarehouse().getOutputNames<MFEMDataCollection>();
  for(const auto& name: mfem_data_collections) {
    _outputs.data_collections[name] = _app.getOutputWarehouse().getOutput<MFEMDataCollection>(name)->_data_collection;
  }

  // _outputs.data_collections["dc_test"] = data_collections[0]->_data_collection;
  hephaestus::Inputs inputs(mfem_mesh, _formulation, _order, _bc_maps, _mat_map, _executioner, _outputs);

  std::vector<char *> argv;
  std::cout << "Launching MFEM solve\n\n" << std::endl;
  run_hephaestus(argv.size() - 1, argv.data(), inputs);
}

void
MFEMProblem::addBoundaryCondition(const std::string & bc_name,
                                  const std::string & name,
                                  InputParameters & parameters)
{
  FEProblemBase::addUserObject(bc_name, name, parameters);
  MFEMBoundaryCondition * mfem_bc(&getUserObject<MFEMBoundaryCondition>(name));
  _bc_maps[name] = mfem_bc->getBC();
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
