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
    params.SetParam("Mesh", mfem::ParMesh(MPI_COMM_WORLD, mfem_mesh));
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
    std::cout << "Launching MFEM solve\n\n" << std::endl;
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
MFEMProblem::setMFEMVarData(EquationSystems & esRef, std::string var_name)
{
  // auto & mooseVarRef = getVariable(0, var_name);
  // MeshBase & libmeshBase = mesh().getMesh();
  // NumericVector<Number> & tempSolutionVector = mooseVarRef.sys().solution();
  // for (int i = 0; i < libmeshBase.n_nodes() /*number of nodes*/; i++)
  // {
  //   Node * nodePtr = libmeshBase.node_ptr(i);
  //   dof_id_type dof = nodePtr->dof_number(mooseVarRef.sys().number(), mooseVarRef.number(), 0);
  //   executioner->variables->gfs.Get(var_name)[0][i] = tempSolutionVector(dof);
  // }
  // mooseVarRef.sys().solution().close();

  // mooseVarRef.sys().update();
}

void
MFEMProblem::setMOOSEVarData(std::string var_name, EquationSystems & esRef)
{
  // auto & mooseVarRef = getVariable(
  //     0, var_name, Moose::VarKindType::VAR_ANY, Moose::VarFieldType::VAR_FIELD_STANDARD);
  // MeshBase & libmeshBase = mesh().getMesh();
  // for (int i = 0; i < libmeshBase.n_nodes(); i++)
  // {
  //   Node * nodePtr = libmeshBase.node_ptr(i);
  //   dof_id_type dof = nodePtr->dof_number(mooseVarRef.sys().number(), mooseVarRef.number(), 0);
  //   mooseVarRef.sys().solution().set(
  //       dof, (executioner->variables->gfs.Get(var_name)[0])[i]); /*Needs to be changed for
  //       tetra*/
  // }
  // mooseVarRef.sys().solution().close();
  // mooseVarRef.sys().update();
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
  // If data is being sent from the master app
  if (direction == Direction::TO_EXTERNAL_APP)
  {
    for (std::string name : getAuxVariableNames())
    {
      setMFEMVarData(es(), name);
      std::cout << name << std::endl;
    }
  }

  // If data is being sent back to master app
  if (direction == Direction::FROM_EXTERNAL_APP)
  {
    for (std::string name : getAuxVariableNames())
    {
      setMOOSEVarData(name, es());
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
