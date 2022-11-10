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
    _variables(),
    _auxkernels(),
    _postprocessors(),
    _exec_params(),
    _outputs()
{
}

void
MFEMProblem::init()
{
  FEProblemBase::init();

  mfem::Mesh & mfem_mesh = *(mesh().mfem_mesh);
  int * partitioning = new int[mesh().getMesh().n_elem()];
  for (auto elem : mesh().getMesh().element_ptr_range())
  {
    unsigned int elem_id = elem->id();
    partitioning[elem_id] = elem->processor_id();
  }

  mfem::ParMesh mfem_parmesh = mfem::ParMesh(MPI_COMM_WORLD, mfem_mesh, partitioning);

  std::vector<OutputName> mfem_data_collections =
      _app.getOutputWarehouse().getOutputNames<MFEMDataCollection>();
  for (const auto & name : mfem_data_collections)
  {
    _outputs.data_collections[name] =
        _app.getOutputWarehouse().getOutput<MFEMDataCollection>(name)->_data_collection;
  }

  if (_formulation != "Joule")
  {
    Transient * _moose_executioner = dynamic_cast<Transient *>(_app.getExecutioner());
    if (_moose_executioner == NULL)
    {
      mooseError("Only Transient Executioners are currently supported by MFEMProblem");
    }
    _exec_params.SetParam("StartTime", float(_moose_executioner->getStartTime()));
    _exec_params.SetParam("TimeStep", float(dt()));
    _exec_params.SetParam("EndTime", float(_moose_executioner->endTime()));
    executioner = new hephaestus::TransientExecutioner(_exec_params);

    hephaestus::InputParameters params;
    params.SetParam("Mesh", mfem_parmesh);
    params.SetParam("Executioner", executioner);
    params.SetParam("Order", 2);
    params.SetParam("BoundaryConditions", _bc_maps);
    params.SetParam("DomainProperties", _domain_properties);
    params.SetParam("Variables", _variables);
    params.SetParam("AuxKernels", _auxkernels);
    params.SetParam("Postprocessors", _postprocessors);
    params.SetParam("Outputs", _outputs);
    params.SetParam("FormulationName", _formulation);

    std::cout << "Launching MFEM solve\n\n" << std::endl;
    executioner->Init(params);
  }
}

void
MFEMProblem::externalSolve()
{
  if (_formulation == "Joule")
  {
    mfem::Mesh & mfem_mesh = *(mesh().mfem_mesh);
    // Legacy support for running Joule solver
    hephaestus::Executioner _executioner(
        std::string("Transient"), getParam<double>("dt"), 0.0, getParam<double>("end_time"));
    hephaestus::Inputs inputs(
        mfem_mesh, _formulation, _order, _bc_maps, _domain_properties, _executioner, _outputs);
    std::vector<char *> argv;
    joule_solve(argv.size() - 1, argv.data(), inputs);
  }
  else
  {
    executioner->Step(dt(), timeStep());
  }
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

    // Hotfix to ensure coupled coeffs get properly initialised in Hephaestus.
    // To replaced by addAuxkernels?
    for (auto coef = mfem_subdomain.property_map.begin(); coef != mfem_subdomain.property_map.end();
         ++coef)
    {
      hephaestus::CoupledCoefficient * _coupled_coef =
          dynamic_cast<hephaestus::CoupledCoefficient *>(coef->second);
      if (_coupled_coef != NULL)
      {
        _auxkernels.Register(coef->first, _coupled_coef, false);
      }
    }

    _domain_properties.subdomains.push_back(mfem_subdomain);
  }
}

void
MFEMProblem::addAuxVariable(const std::string & var_type,
                            const std::string & var_name,
                            InputParameters & parameters)
{
  if (var_type == "MFEMVariable")
  {
    FEProblemBase::addUserObject(var_type, var_name, parameters);
  }
  else
  {
    ExternalProblem::addAuxVariable(var_type, var_name, parameters);

    InputParameters mfemvar_params(getMFEMVarParamsFromMOOSEVarParams(parameters));
    FEProblemBase::addUserObject("MFEMVariable", var_name, mfemvar_params);
  }

  MFEMVariable & var(getUserObject<MFEMVariable>(var_name));
  var.mfem_params.SetParam("VariableName", var_name);
  _variables.AddVariable(var.mfem_params);
}

void
MFEMProblem::addAuxKernel(const std::string & kernel_name,
                          const std::string & name,
                          InputParameters & parameters)
{
  FEProblemBase::addUserObject(kernel_name, name, parameters);
  MFEMAuxKernel * mfem_auxkernel(&getUserObject<MFEMAuxKernel>(name));

  _auxkernels.Register(name, mfem_auxkernel->getAuxKernel(), true);
  mfem_auxkernel->storeCoefficients(_domain_properties);
}

void
MFEMProblem::setMFEMVarData(EquationSystems & esRef,
                            std::string var_name,
                            std::map<int, int> libmeshToMFEMNode)
{

  auto & mooseVarRef = getVariable(0, var_name);
  MeshBase & libmeshBase = mesh().getMesh();
  unsigned int order = (unsigned int)mooseVarRef.order();
  NumericVector<Number> & tempSolutionVector = mooseVarRef.sys().solution();
  auto & pgf = *(executioner->variables->gfs.Get(var_name));
  mfem::Vector mfem_local_nodes(libmeshBase.n_local_nodes());
  mfem::Vector mfem_local_elems(libmeshBase.n_local_elem());
  unsigned int count = 0;
  if (mooseVarRef.isNodal())
  {
    for (auto & node : as_range(libmeshBase.local_nodes_begin(), libmeshBase.local_nodes_end()))
    {
      unsigned int n_comp = node->n_comp(mooseVarRef.sys().number(), mooseVarRef.number());
      unsigned int node_id = node->id();
      for (unsigned int i = 0; i < n_comp; i++)
      {
        dof_id_type dof = node->dof_number(mooseVarRef.sys().number(), mooseVarRef.number(), i);

        if (order == 1)
        {
          mfem_local_nodes[count] = tempSolutionVector(dof);
        }
        else
        {
          // mfem_local_nodes[libmeshToMFEMNode[node_id]] = tempSolutionVector(dof);
        }
        count++;
      }
    }
    pgf.SetFromTrueDofs(mfem_local_nodes);
  }
  else
  {
    for (auto & elem :
         as_range(libmeshBase.local_elements_begin(), libmeshBase.local_elements_end()))
    {
      unsigned int n_comp = elem->n_comp(mooseVarRef.sys().number(), mooseVarRef.number());
      unsigned int elem_id = elem->id();
      for (unsigned int i = 0; i < n_comp; i++)
      {
        dof_id_type dof = elem->dof_number(mooseVarRef.sys().number(), mooseVarRef.number(), i);
        mfem_local_elems[count] = tempSolutionVector(dof);
        count++;
      }
    }
    pgf.SetFromTrueDofs(mfem_local_elems);
  }
  mooseVarRef.sys().solution().close();
  mooseVarRef.sys().update();
}

void
MFEMProblem::setMOOSEVarData(std::string var_name,
                             EquationSystems & esRef,
                             std::map<int, int> libmeshToMFEMNode)
{
  auto & mooseVarRef = getVariable(0, var_name);
  MeshBase & libmeshBase = mesh().getMesh();
  unsigned int order = (unsigned int)mooseVarRef.order();
  NumericVector<Number> & tempSolutionVector = mooseVarRef.sys().solution();
  auto & pgf = *(executioner->variables->gfs.Get(var_name));

  unsigned int count = 0;

  if (mooseVarRef.isNodal())
  {
    mfem::Vector mfem_local_nodes(libmeshBase.n_local_nodes());
    mfem_local_nodes = *(pgf.GetTrueDofs());
    for (auto & node : libmeshBase.local_node_ptr_range())
    {
      unsigned int n_comp = node->n_comp(mooseVarRef.sys().number(), mooseVarRef.number());
      unsigned int node_id = node->id();
      for (unsigned int i = 0; i < n_comp; i++)
      {
        dof_id_type dof = node->dof_number(mooseVarRef.sys().number(), mooseVarRef.number(), i);
        if (order == 1)
        {
          mooseVarRef.sys().solution().set(dof, mfem_local_nodes[count]);
        }
        else
        {
          // mooseVarRef.sys().solution().set(
          //     dof, (executioner->variables->gfs.Get(var_name)[0])[libmeshToMFEMNode[node_id]]);
        }
      }
      count++;
    }
  }
  else
  {
    mfem::Vector mfem_local_elems(libmeshBase.n_local_elem());
    mfem_local_elems = *(pgf.GetTrueDofs());
    for (auto & elem :
         as_range(libmeshBase.local_elements_begin(), libmeshBase.local_elements_end()))
    {
      unsigned int n_comp = elem->n_comp(mooseVarRef.sys().number(), mooseVarRef.number());
      unsigned int elem_id = elem->id();
      for (unsigned int i = 0; i < n_comp; i++)
      {
        dof_id_type dof = elem->dof_number(mooseVarRef.sys().number(), mooseVarRef.number(), i);
        mooseVarRef.sys().solution().set(dof, mfem_local_elems[count]);
        count++;
      }
    }
  }
  mooseVarRef.sys().solution().close();
  mooseVarRef.sys().update();
}

InputParameters
MFEMProblem::getMFEMVarParamsFromMOOSEVarParams(InputParameters & moosevar_params)
{
  std::string var_family = moosevar_params.get<MooseEnum>("family");

  InputParameters mfemvar_params = _factory.getValidParams("MFEMVariable");
  mfemvar_params.setParameters<MooseEnum>("order", moosevar_params.get<MooseEnum>("order"));
  if (var_family == "LAGRANGE")
  {
    mfemvar_params.set<MooseEnum>("fespace") = std::string("H1");
  }
  if (var_family == "MONOMIAL")
  {
    mfemvar_params.set<MooseEnum>("fespace") = std::string("L2");
  }

  return mfemvar_params;
}

std::vector<VariableName>
MFEMProblem::getAuxVariableNames()
{
  return systemBaseAuxiliary().getVariableNames();
}

void
MFEMProblem::syncSolutions(Direction direction)
{
  // Only sync solutions if MOOSE and MFEM meshes are coupled
  if (ExternalProblem::mesh().type() == "CoupledMFEMMesh")
  {
    // Map for second order var transfer;
    std::map<int, int> * libmeshToMFEMNodePtr;

    auto & coupledMesh = dynamic_cast<CoupledMFEMMesh &>(mesh());
    libmeshToMFEMNodePtr = &(coupledMesh.libmeshToMFEMNode);

    // If data is being sent from the master app
    if (direction == Direction::TO_EXTERNAL_APP)
    {
      for (std::string name : getAuxVariableNames())
      {
        setMFEMVarData(es(), name, (*libmeshToMFEMNodePtr));
      }
    }

    // If data is being sent back to master app
    if (direction == Direction::FROM_EXTERNAL_APP)
    {
      for (std::string name : getAuxVariableNames())
      {
        setMOOSEVarData(name, es(), (*libmeshToMFEMNodePtr));
      }
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
