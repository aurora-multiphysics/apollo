#include "MFEMFunctionDirichletBC.h"
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
    _domain_properties(),
    _executioner(
        std::string("Transient"), getParam<double>("dt"), 0.0, getParam<double>("end_time")),
    _outputs()
{
}

void
MFEMProblem::externalSolve()
{
  mfem::Mesh & mfem_mesh = *(mesh().mfem_mesh);
  std::vector<OutputName> mfem_data_collections =
      _app.getOutputWarehouse().getOutputNames<MFEMDataCollection>();
  for (const auto & name : mfem_data_collections)
  {
    _outputs.data_collections[name] =
        _app.getOutputWarehouse().getOutput<MFEMDataCollection>(name)->_data_collection;
  }

  hephaestus::Inputs inputs(
      mfem_mesh, _formulation, _order, _bc_maps, _domain_properties, _executioner, _outputs);

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
  mfem_bc->storeCoefficients(_domain_properties);
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
    _domain_properties.subdomains.push_back(mfem_subdomain);
  }
}

void
MFEMProblem::addAuxVariable(const std::string & var_type,
                            const std::string & var_name,
                            InputParameters & parameters)
{
  // Standard Moose implementation
  auto var_order = Utility::string_to_enum<Order>(parameters.get<MooseEnum>("order"));
  std::string var_family = parameters.get<MooseEnum>("family");

  ExternalProblem::addAuxVariable(var_type, var_name, parameters);
  // End of standard implementation

  // New code to create MFEM grid functions
  mfem::Mesh * mfem_mesh = mesh().mfem_mesh;
  mfem::FiniteElementCollection * fec = fecGet(var_family);
  mfem::FiniteElementSpace fespace(mfem_mesh, fec);
  hephaestus::AuxiliaryVariable * var =
      new hephaestus::AuxiliaryVariable(var_name, var_family, (int)var_order, fespace);
  _var_map.insert(std::pair<std::string, hephaestus::AuxiliaryVariable *>(var_name, var));
}

void
MFEMProblem::setMFEMVarData(EquationSystems & esRef, hephaestus::AuxiliaryVariable * var)
{
  auto & mooseVarRef = getVariable(0, var->name);
  MeshBase & libmeshBase = mesh().getMesh();
  NumericVector<Number> & tempSolutionVector = mooseVarRef.sys().solution();
  for (int i = 0; i < libmeshBase.n_nodes() /*number of nodes*/; i++)
  {
    Node * nodePtr = libmeshBase.node_ptr(i);
    dof_id_type dof = nodePtr->dof_number(mooseVarRef.sys().number(), mooseVarRef.number(), 0);
    var->gf[i] = tempSolutionVector(dof);
  }
  mooseVarRef.sys().solution().close();
  mooseVarRef.sys().update();
}

void
MFEMProblem::setMOOSEVarData(hephaestus::AuxiliaryVariable * var, EquationSystems & esRef)
{
  auto & mooseVarRef = getVariable(
      0, var->name, Moose::VarKindType::VAR_ANY, Moose::VarFieldType::VAR_FIELD_STANDARD);
  MeshBase & libmeshBase = mesh().getMesh();
  for (int i = 0; i < libmeshBase.n_nodes(); i++)
  {
    Node * nodePtr = libmeshBase.node_ptr(i);
    dof_id_type dof = nodePtr->dof_number(mooseVarRef.sys().number(), mooseVarRef.number(), 0);
    mooseVarRef.sys().solution().set(dof, var->gf[i]); /*Needs to be changed for tetra*/
  }
  mooseVarRef.sys().solution().close();
  mooseVarRef.sys().update();
}

mfem::FiniteElementCollection *
MFEMProblem::fecGet(std::string var_fam)
{
  mfem::Mesh * mfem_mesh = mesh().mfem_mesh;
  mfem::FiniteElementCollection * fecPtr;
  std::cout << "Variable family = " << var_fam << std::endl;

  if (var_fam == "LAGRANGE")
  {
    mfem::H1_FECollection * fec = new mfem::H1_FECollection(_order, mfem_mesh->Dimension());
    fecPtr = dynamic_cast<mfem::FiniteElementCollection *>(fec);
  }

  if (var_fam == "MONOMIAL")
  {
    mfem::L2_FECollection * fec = new mfem::L2_FECollection(_order, mfem_mesh->Dimension());
    fecPtr = dynamic_cast<mfem::FiniteElementCollection *>(fec);
  }
  // More types need adding, I need to understand what types are analogous
  return fecPtr;
}

std::vector<VariableName>
MFEMProblem::getAuxVariableNames()
{
  return systemBaseAuxiliary().getVariableNames();
}

void
MFEMProblem::syncSolutions(Direction direction)
{
  // If data is being sent from the master app
  if (direction == Direction::TO_EXTERNAL_APP)
  {
    for (std::string name : getAuxVariableNames())
    {
      setMFEMVarData(es(), _var_map[name]);
      std::cout << name << std::endl;
    }
  }

  // If data is being sent back to master app
  if (direction == Direction::FROM_EXTERNAL_APP)
  {
    for (std::string name : getAuxVariableNames())
    {
      setMOOSEVarData(_var_map[name], es());
    }
  }
}

ExclusiveMFEMMesh &
MFEMProblem::mesh()
{
  if (ExternalProblem::mesh().type() != "ExclusiveMFEMMesh" &&
      ExternalProblem::mesh().type() != "CoupledMFEMMesh")
  {
    std::cout << "Please choose a valid mesh type for an MFEMProblem\n(Either CoupledMFEMMesh or "
                 "ExclusiveMFEMMesh)"
              << std::endl;
    ExternalProblem::mesh().mooseError();
  }
  return (ExclusiveMFEMMesh &)_mesh;
}
