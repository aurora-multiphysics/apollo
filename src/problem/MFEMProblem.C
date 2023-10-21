#include "MFEMProblem.h"

registerMooseObject("ApolloApp", MFEMProblem);

InputParameters
MFEMProblem::validParams()
{
  InputParameters params = ExternalProblem::validParams();
  params.addParam<int>(
      "vis_steps",
      1,
      "Number of timesteps between successive write outs of data collections to file.");
  params.addParam<bool>(
      "use_glvis", false, "Attempt to open GLVis ports to display variables during simulation");

  return params;
}

MFEMProblem::MFEMProblem(const InputParameters & params)
  : ExternalProblem(params),
    _input_mesh(_mesh.parameters().get<MeshFileName>("file")),
    _bc_maps(),
    _coefficients(),
    _fespaces(),
    _gridfunctions(),
    _preprocessors(),
    _postprocessors(),
    _sources(),
    _outputs(),
    _exec_params()
{
  std::cout << "Problem initialised\n\n" << std::endl;
}

MFEMProblem::~MFEMProblem()
{
  if (executioner != NULL)
  {
    delete executioner;
  }
}

void
MFEMProblem::initialSetup()
{
  FEProblemBase::initialSetup();

  std::vector<OutputName> mfem_data_collections =
      _app.getOutputWarehouse().getOutputNames<MFEMDataCollection>();
  for (const auto & name : mfem_data_collections)
  {
    _outputs.data_collections[name] =
        _app.getOutputWarehouse().getOutput<MFEMDataCollection>(name)->_data_collection;
  }

  EquationSystems & es = FEProblemBase::es();
  _solver_options.SetParam("Tolerance", float(es.parameters.get<Real>("linear solver tolerance")));
  _solver_options.SetParam("MaxIter",
                           es.parameters.get<unsigned int>("linear solver maximum iterations"));
  _solver_options.SetParam("PrintLevel", -1);

  std::cout << "Launching MFEM solve\n\n" << std::endl;
  // mfem_problem_builder->SetFESpaces(_fespaces);
  // mfem_problem_builder->SetGridFunctions(_gridfunctions);
  _coefficients.AddGlobalCoefficientsFromSubdomains();
  mfem_problem_builder->SetBoundaryConditions(_bc_maps);
  mfem_problem_builder->SetAuxSolvers(_preprocessors);
  mfem_problem_builder->SetCoefficients(_coefficients);
  mfem_problem_builder->SetPostprocessors(_postprocessors);
  mfem_problem_builder->SetSources(_sources);
  mfem_problem_builder->SetOutputs(_outputs);
  mfem_problem_builder->SetSolverOptions(_solver_options);

  mfem_problem_builder->RegisterFESpaces();
  mfem_problem_builder->RegisterGridFunctions();
  mfem_problem_builder->RegisterAuxSolvers();
  mfem_problem_builder->RegisterCoefficients();

  mfem_problem_builder->InitializePostprocessors();
  mfem_problem_builder->InitializeKernels();
  mfem_problem_builder->ConstructOperator();
  mfem_problem_builder->ConstructState();
  mfem_problem_builder->ConstructSolver();

  // hephaestus::ProblemBuildSequencer sequencer(mfem_problem_builder);
  // sequencer.ConstructEquationSystemProblem();
  hephaestus::InputParameters exec_params;

  Transient * _moose_executioner = dynamic_cast<Transient *>(_app.getExecutioner());
  if (_moose_executioner != NULL)
  {
    mfem_problem =
        dynamic_cast<hephaestus::TimeDomainProblemBuilder *>(mfem_problem_builder)->ReturnProblem();

    exec_params.SetParam("StartTime", float(_moose_executioner->getStartTime()));
    exec_params.SetParam("TimeStep", float(dt()));
    exec_params.SetParam("EndTime", float(_moose_executioner->endTime()));
    exec_params.SetParam("VisualisationSteps", getParam<int>("vis_steps"));
    exec_params.SetParam("UseGLVis", getParam<bool>("use_glvis"));
    exec_params.SetParam("Problem",
                         dynamic_cast<hephaestus::TimeDomainProblem *>(mfem_problem.get()));
    executioner = new hephaestus::TransientExecutioner(exec_params);
  }
  else if (dynamic_cast<Steady *>(_app.getExecutioner()))
  {
    mfem_problem = dynamic_cast<hephaestus::SteadyStateProblemBuilder *>(mfem_problem_builder)
                       ->ReturnProblem();

    exec_params.SetParam("UseGLVis", getParam<bool>("use_glvis"));
    exec_params.SetParam("Problem",
                         dynamic_cast<hephaestus::SteadyStateProblem *>(mfem_problem.get()));
    executioner = new hephaestus::SteadyExecutioner(exec_params);
  }
  else
  {
    mooseError("Executioner used that is not currently supported by MFEMProblem");
  }
  executioner->Init();
}

void
MFEMProblem::init()
{
  FEProblemBase::init();
}

void
MFEMProblem::externalSolve()
{
  hephaestus::TransientExecutioner * transient_mfem_exec =
      dynamic_cast<hephaestus::TransientExecutioner *>(executioner);
  if (transient_mfem_exec != NULL)
  {
    transient_mfem_exec->t_step = dt();
  }

  executioner->Solve();
}

void
MFEMProblem::setFormulation(const std::string & user_object_name,
                            const std::string & name,
                            InputParameters & parameters)
{
  mfem::ParMesh & mfem_par_mesh = mesh().getMFEMParMesh();

  FEProblemBase::addUserObject(user_object_name, name, parameters);
  MFEMFormulation * mfem_formulation(&getUserObject<MFEMFormulation>(name));
  mfem_problem_builder = mfem_formulation->getProblemBuilder();
  mfem_problem_builder->ConstructEquationSystem();
  mfem_problem_builder->SetMesh(std::make_shared<mfem::ParMesh>(mfem_par_mesh));
}

void
MFEMProblem::addBoundaryCondition(const std::string & bc_name,
                                  const std::string & name,
                                  InputParameters & parameters)
{
  FEProblemBase::addUserObject(bc_name, name, parameters);
  MFEMBoundaryCondition * mfem_bc(&getUserObject<MFEMBoundaryCondition>(name));
  _bc_maps.Register(name, mfem_bc->getBC(), false);
  mfem_bc->storeCoefficients(_coefficients);
}

void
MFEMProblem::addMaterial(const std::string & kernel_name,
                         const std::string & name,
                         InputParameters & parameters)
{
  FEProblemBase::addUserObject(kernel_name, name, parameters);
  MFEMMaterial & mfem_material(getUserObject<MFEMMaterial>(name));

  for (unsigned int bid = 0; bid < mfem_material.blocks.size(); ++bid)
  {
    int block = std::stoi(mfem_material.blocks[bid]);
    hephaestus::Subdomain mfem_subdomain(name, block);
    mfem_material.storeCoefficients(mfem_subdomain);

    // Hotfix to ensure coupled coeffs get properly initialised in Hephaestus.
    // To replaced by addAuxkernels?
    for (auto coef = mfem_subdomain.scalar_coefficients.begin();
         coef != mfem_subdomain.scalar_coefficients.end();
         ++coef)
    {
      hephaestus::CoupledCoefficient * _coupled_coef =
          dynamic_cast<hephaestus::CoupledCoefficient *>(coef->second);
      if (_coupled_coef != NULL)
      {
        _preprocessors.Register(coef->first, _coupled_coef, false);
      }
    }

    _coefficients.subdomains.push_back(mfem_subdomain);
  }
}

void
MFEMProblem::addSource(const std::string & user_object_name,
                       const std::string & name,
                       InputParameters & parameters)
{
  FEProblemBase::addUserObject(user_object_name, name, parameters);
  MFEMSource * mfem_source(&getUserObject<MFEMSource>(name));
  _sources.Register(name, mfem_source->getSource(), true);
  mfem_source->storeCoefficients(_coefficients);
}

void
MFEMProblem::addCoefficient(const std::string & user_object_name,
                            const std::string & name,
                            InputParameters & parameters)
{
  FEProblemBase::addUserObject(user_object_name, name, parameters);
  mfem::Coefficient * mfem_coef(&getUserObject<mfem::Coefficient>(name));
  _coefficients.scalars.Register(name, mfem_coef, true);
}

void
MFEMProblem::addFESpace(const std::string & user_object_name,
                        const std::string & name,
                        InputParameters & parameters)
{
  FEProblemBase::addUserObject(user_object_name, name, parameters);
  MFEMFESpace & mfem_fespace(getUserObject<MFEMFESpace>(name));
  mfem_problem_builder->AddFESpace(name, mfem_fespace.fec_name, mfem_fespace.vdim);
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
    InputParameters mfem_var_params(addMFEMFESpaceFromMOOSEVariable(parameters));
    FEProblemBase::addUserObject("MFEMVariable", var_name, mfem_var_params);
  }
  MFEMVariable & var(getUserObject<MFEMVariable>(var_name));
  mfem_problem_builder->AddGridFunction(var_name, var.fespace.name());
}

void
MFEMProblem::addKernel(const std::string & kernel_name,
                       const std::string & name,
                       InputParameters & parameters)
{
  FEProblemBase::addUserObject(kernel_name, name, parameters);
  const UserObject * kernel = &(getUserObjectBase(name));

  if (dynamic_cast<const MFEMLinearFormKernel *>(kernel) != nullptr)
  {
    MFEMLinearFormKernel * lf_kernel(&getUserObject<MFEMLinearFormKernel>(name));
    mfem_problem_builder->AddKernel<mfem::ParLinearForm>(parameters.get<std::string>("variable"),
                                                         lf_kernel->getKernel());
  }
  else if (dynamic_cast<const MFEMBilinearFormKernel *>(kernel) != nullptr)
  {
    MFEMBilinearFormKernel * blf_kernel(&getUserObject<MFEMBilinearFormKernel>(name));
    mfem_problem_builder->AddKernel<mfem::ParBilinearForm>(parameters.get<std::string>("variable"),
                                                           blf_kernel->getKernel());
  }
}

void
MFEMProblem::addAuxKernel(const std::string & kernel_name,
                          const std::string & name,
                          InputParameters & parameters)
{
  FEProblemBase::addUserObject(kernel_name, name, parameters);
  MFEMAuxSolver * mfem_auxsolver(&getUserObject<MFEMAuxSolver>(name));

  _postprocessors.Register(name, mfem_auxsolver->getAuxSolver(), true);
  mfem_auxsolver->storeCoefficients(_coefficients);
}

void
MFEMProblem::setMFEMVarData(std::string var_name, EquationSystems & esRef)
{

  auto & mooseVarRef = getVariable(0, var_name);
  MeshBase & libmeshBase = mesh().getMesh();
  unsigned int order = (unsigned int)mooseVarRef.order();
  NumericVector<Number> & tempSolutionVector = mooseVarRef.sys().solution();
  auto & pgf = *(mfem_problem->gridfunctions.Get(var_name));
  mfem::Vector mfem_local_nodes(libmeshBase.n_local_nodes());
  mfem::Vector mfem_local_elems(libmeshBase.n_local_elem());
  unsigned int count = 0;
  if (mooseVarRef.isNodal())
  {
    for (auto & node : as_range(libmeshBase.local_nodes_begin(), libmeshBase.local_nodes_end()))
    {
      if (node->n_dofs(mooseVarRef.sys().number(), mooseVarRef.number()) >
          0) // If this variable has dofs at this node
      {
        unsigned int n_comp = node->n_comp(mooseVarRef.sys().number(), mooseVarRef.number());
        unsigned int node_id = node->id();
        for (unsigned int i = 0; i < n_comp; i++)
        {
          count++;
        }
      }
    }
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
        count++;
      }
    }
  }
  mfem::Vector mfem_local_dofs(count);

  count = 0;
  if (mooseVarRef.isNodal())
  {
    for (auto & node : as_range(libmeshBase.local_nodes_begin(), libmeshBase.local_nodes_end()))
    {
      if (node->n_dofs(mooseVarRef.sys().number(), mooseVarRef.number()) >
          0) // If this variable has dofs at this node
      {
        unsigned int n_comp = node->n_comp(mooseVarRef.sys().number(), mooseVarRef.number());
        unsigned int node_id = node->id();
        for (unsigned int i = 0; i < n_comp; i++)
        {
          dof_id_type dof = node->dof_number(mooseVarRef.sys().number(), mooseVarRef.number(), i);

          if (order == 1)
          {
            mfem_local_dofs[count] = tempSolutionVector(dof);
          }
          else
          {
            // mfem_local_nodes[libmeshToMFEMNode[node_id]] = tempSolutionVector(dof);
          }
          count++;
        }
      }
    }
    pgf.SetFromTrueDofs(mfem_local_dofs);
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
        mfem_local_dofs[count] = tempSolutionVector(dof);
        count++;
      }
    }
    pgf.SetFromTrueDofs(mfem_local_dofs);
  }
  mooseVarRef.sys().solution().close();
  mooseVarRef.sys().update();
}

void
MFEMProblem::setMOOSEVarData(std::string var_name, EquationSystems & esRef)
{
  auto & mooseVarRef = getVariable(0, var_name);
  MeshBase & libmeshBase = mesh().getMesh();
  unsigned int order = (unsigned int)mooseVarRef.order();
  NumericVector<Number> & tempSolutionVector = mooseVarRef.sys().solution();
  auto & pgf = *(mfem_problem->gridfunctions.Get(var_name));

  unsigned int count = 0;

  if (mooseVarRef.isNodal())
  {
    mfem::Vector mfem_local_nodes(libmeshBase.n_local_nodes());
    mfem_local_nodes = *(pgf.GetTrueDofs());
    for (auto & node : as_range(libmeshBase.local_nodes_begin(), libmeshBase.local_nodes_end()))
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
        count++;
      }
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
MFEMProblem::addMFEMFESpaceFromMOOSEVariable(InputParameters & moosevar_params)
{
  InputParameters mfem_fespace_params = _factory.getValidParams("MFEMFESpace");
  mfem_fespace_params.setParameters<MooseEnum>("order", moosevar_params.get<MooseEnum>("order"));
  InputParameters mfem_var_params = _factory.getValidParams("MFEMVariable");
  std::string var_family = moosevar_params.get<MooseEnum>("family");
  if (var_family == "LAGRANGE")
    mfem_fespace_params.set<MooseEnum>("fespace_type") = std::string("H1");
  if (var_family == "MONOMIAL")
    mfem_fespace_params.set<MooseEnum>("fespace_type") = std::string("L2");
  addFESpace("MFEMFESpace", var_family, mfem_fespace_params);
  mfem_var_params.set<UserObjectName>("fespace") = var_family;
  return mfem_var_params;
}

std::vector<VariableName>
MFEMProblem::getAuxVariableNames()
{
  return systemBaseAuxiliary().getVariableNames();
}

void
MFEMProblem::syncSolutions(Direction direction)
{
  // Only sync solutions if MOOSE and MFEM meshes are coupled.
  if (ExternalProblem::mesh().type() != "CoupledMFEMMesh")
  {
    return;
  }

  void (MFEMProblem::*setVarDataFuncPtr)(std::string, EquationSystems &);

  switch (direction)
  {
    case Direction::TO_EXTERNAL_APP:
      setVarDataFuncPtr =
          &MFEMProblem::setMFEMVarData; // If data is being sent from the master app.
      break;
    case Direction::FROM_EXTERNAL_APP:
      setVarDataFuncPtr =
          &MFEMProblem::setMOOSEVarData; // If data is being sent back to the master app.
      break;
    default:
      setVarDataFuncPtr = nullptr;
      break;
  }

  if (!setVarDataFuncPtr)
  {
    mooseError("Invalid syncSolutions direction specified.");
  }

  for (std::string name : getAuxVariableNames())
  {
    (this->*setVarDataFuncPtr)(name, es());
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
