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
    _coefficients(),
    _outputs(),
    _exec_params()
{
}

MFEMProblem::~MFEMProblem()
{
  if (executioner != NULL)
  {
    delete executioner;
  }
}

void
MFEMProblem::outputStep(ExecFlagType type)
{
  // Needed to ensure outputs from successive runs when using MultiApps are stored in
  // directories with iterated names
  if (type == EXEC_INITIAL)
  {
    std::vector<OutputName> mfem_data_collections =
        _app.getOutputWarehouse().getOutputNames<MFEMDataCollection>();
    for (const auto & name : mfem_data_collections)
    {
      auto dc = _app.getOutputWarehouse().getOutput<MFEMDataCollection>(name);
      int filenum(dc->getFileNumber());
      std::string filename("/Run" + std::to_string(filenum));

      mfem_problem->outputs.Register(name, dc->createDataCollection(filename), true);
      mfem_problem->outputs.Reset();
      dc->setFileNumber(filenum + 1);
    }
  }
  FEProblemBase::outputStep(type);
}

void
MFEMProblem::initialSetup()
{
  FEProblemBase::initialSetup();
  EquationSystems & es = FEProblemBase::es();
  _solver_options.SetParam("Tolerance", float(es.parameters.get<Real>("linear solver tolerance")));
  _solver_options.SetParam("AbsTolerance",
                           float(es.parameters.get<Real>("linear solver absolute tolerance")));
  _solver_options.SetParam("MaxIter",
                           es.parameters.get<unsigned int>("linear solver maximum iterations"));
  _solver_options.SetParam("PrintLevel", 2);
  _coefficients.AddGlobalCoefficientsFromSubdomains();
  mfem_problem_builder->SetCoefficients(_coefficients);
  mfem_problem_builder->SetSolverOptions(_solver_options);

  mfem_problem_builder->RegisterFESpaces();
  mfem_problem_builder->RegisterGridFunctions();
  mfem_problem_builder->RegisterAuxSolvers();
  mfem_problem_builder->RegisterCoefficients();

  mfem_problem_builder->InitializeAuxSolvers();
  mfem_problem_builder->InitializeKernels();
  mfem_problem_builder->InitializeOutputs();
  mfem_problem_builder->ConstructOperator();
  mfem_problem_builder->ConstructState();
  mfem_problem_builder->ConstructSolver();

  hephaestus::InputParameters exec_params;

  Transient * _moose_executioner = dynamic_cast<Transient *>(_app.getExecutioner());
  if (_moose_executioner != NULL)
  {
    hephaestus::TimeDomainProblemBuilder * mfem_transient_problem_builder =
        dynamic_cast<hephaestus::TimeDomainProblemBuilder *>(mfem_problem_builder);
    if (mfem_transient_problem_builder == NULL)
      mooseError("Specified formulation does not support Transient executioners");
    mfem_problem = mfem_transient_problem_builder->ReturnProblem();
    exec_params.SetParam("StartTime", float(_moose_executioner->getStartTime()));
    exec_params.SetParam("TimeStep", float(dt()));
    exec_params.SetParam("EndTime", float(_moose_executioner->endTime()));
    exec_params.SetParam("VisualisationSteps", getParam<int>("vis_steps"));
    exec_params.SetParam("Problem",
                         dynamic_cast<hephaestus::TimeDomainProblem *>(mfem_problem.get()));
    executioner = new hephaestus::TransientExecutioner(exec_params);
  }
  else if (dynamic_cast<Steady *>(_app.getExecutioner()))
  {
    hephaestus::SteadyStateProblemBuilder * mfem_steady_problem_builder =
        dynamic_cast<hephaestus::SteadyStateProblemBuilder *>(mfem_problem_builder);
    if (mfem_steady_problem_builder == NULL)
      mooseError("Specified formulation does not support Steady executioners");
    mfem_problem = mfem_steady_problem_builder->ReturnProblem();
    exec_params.SetParam("Problem",
                         dynamic_cast<hephaestus::SteadyStateProblem *>(mfem_problem.get()));
    executioner = new hephaestus::SteadyExecutioner(exec_params);
  }
  else
  {
    mooseError("Executioner used that is not currently supported by MFEMProblem");
  }
  mfem_problem->outputs.EnableGLVis(getParam<bool>("use_glvis"));
}

void
MFEMProblem::init()
{
  FEProblemBase::init();
}

void
MFEMProblem::externalSolve()
{
  if (!_solve)
  {
    return;
  }

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
  mfem_problem_builder->AddBoundaryCondition(name, mfem_bc->getBC(), false);
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
        mfem_problem_builder->AddAuxSolver(coef->first, _coupled_coef, false);
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
  mfem_problem_builder->AddSource(name, mfem_source->getSource(), true);
  mfem_source->storeCoefficients(_coefficients);
}

void
MFEMProblem::addCoefficient(const std::string & user_object_name,
                            const std::string & name,
                            InputParameters & parameters)
{
  FEProblemBase::addUserObject(user_object_name, name, parameters);
  MFEMCoefficient * mfem_coef(&getUserObject<MFEMCoefficient>(name));
  _coefficients.scalars.Register(name, mfem_coef->getCoefficient(), false);
}

void
MFEMProblem::addVectorCoefficient(const std::string & user_object_name,
                                  const std::string & name,
                                  InputParameters & parameters)
{
  FEProblemBase::addUserObject(user_object_name, name, parameters);
  MFEMVectorCoefficient * mfem_vec_coef(&getUserObject<MFEMVectorCoefficient>(name));
  _coefficients.vectors.Register(name, mfem_vec_coef->getVectorCoefficient(), false);
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
  else
  {
    mooseError("Unsupported kernel of type '", kernel_name, "' and name '", name, "' detected.");
  }
}

void
MFEMProblem::addAuxKernel(const std::string & kernel_name,
                          const std::string & name,
                          InputParameters & parameters)
{
  std::string base_auxkernel = parameters.get<std::string>("_moose_base");

  if (base_auxkernel == "MFEMAuxKernel") // MFEM auxsolver.
  {
    FEProblemBase::addUserObject(kernel_name, name, parameters);
    MFEMAuxSolver * mfem_auxsolver(&getUserObject<MFEMAuxSolver>(name));

    mfem_problem_builder->AddPostprocessor(name, mfem_auxsolver->getAuxSolver(), true);
    mfem_auxsolver->storeCoefficients(_coefficients);
  }
  else if (base_auxkernel == "AuxKernel" || base_auxkernel == "VectorAuxKernel" ||
           base_auxkernel == "ArrayAuxKernel") // MOOSE auxkernels.
  {
    FEProblemBase::addAuxKernel(kernel_name, name, parameters);
  }
  else
  {
    mooseError("Unrecognized auxkernel base class '", base_auxkernel, "' detected.");
  }
}

void
MFEMProblem::setMFEMVarData(std::string var_name, EquationSystems & esRef)
{
  auto & moose_var_ref = getVariable(0, var_name);
  MeshBase & libmesh_base = mesh().getMesh();

  unsigned int order = (unsigned int)moose_var_ref.order();

  NumericVector<Number> & temp_solution_vector = moose_var_ref.sys().solution();

  auto & pgf = *(mfem_problem->gridfunctions.Get(var_name));

  const auto * par_fespace = order > 1 ? pgf.ParFESpace() : nullptr;

  unsigned int count = 0;

  if (moose_var_ref.isNodal())
  {
    for (auto & node : as_range(libmesh_base.local_nodes_begin(), libmesh_base.local_nodes_end()))
    {
      bool has_dofs_at_node =
          (node->n_dofs(moose_var_ref.sys().number(), moose_var_ref.number()) > 0);
      if (has_dofs_at_node)
      {
        unsigned int n_comp = node->n_comp(moose_var_ref.sys().number(), moose_var_ref.number());
        count += n_comp;
      }
    }
  }
  else
  {
    for (auto & elem :
         as_range(libmesh_base.local_elements_begin(), libmesh_base.local_elements_end()))
    {
      unsigned int n_comp = elem->n_comp(moose_var_ref.sys().number(), moose_var_ref.number());
      count += n_comp;
    }
  }

  mfem::Vector mfem_true_local_dofs(count);

  count = 0;
  if (moose_var_ref.isNodal())
  {
    for (auto & node : as_range(libmesh_base.local_nodes_begin(), libmesh_base.local_nodes_end()))
    {
      if (node->n_dofs(moose_var_ref.sys().number(), moose_var_ref.number()) >
          0) // If this variable has dofs at this node
      {
        unsigned int n_comp = node->n_comp(moose_var_ref.sys().number(), moose_var_ref.number());
        unsigned int node_id = node->id();
        for (unsigned int i = 0; i < n_comp; i++)
        {
          dof_id_type dof =
              node->dof_number(moose_var_ref.sys().number(), moose_var_ref.number(), i);

          if (order == 1)
          {
            mfem_true_local_dofs[count] = temp_solution_vector(dof);
          }
          else
          {
            // Get the local dof:
            const int mfem_local_dof = mesh().getLocalMFEMNodeId(node_id);

            // Convert to TRUE LOCAL dof. Removes duplicated. No longer 1:1 mapping.
            const int mfem_true_local_dof = par_fespace->GetLocalTDofNumber(mfem_local_dof);

            // Not on this processor? --> skip.
            if (mfem_true_local_dof == -1)
            {
              continue;
            }

            // Catch dodgy local true dofs.
            mooseAssert(mfem_true_local_dof >= 0 &&
                            mfem_true_local_dof < mfem_true_local_dofs.Size(),
                        "local-true dof out-of-bounds.");

            mfem_true_local_dofs[mfem_true_local_dof] = temp_solution_vector(dof);
          }

          count++;
        }
      }
    }

    pgf.SetFromTrueDofs(mfem_true_local_dofs);
  }
  else
  {
    for (auto & elem :
         as_range(libmesh_base.local_elements_begin(), libmesh_base.local_elements_end()))
    {
      unsigned int n_comp = elem->n_comp(moose_var_ref.sys().number(), moose_var_ref.number());
      unsigned int elem_id = elem->id();
      for (unsigned int i = 0; i < n_comp; i++)
      {
        dof_id_type dof = elem->dof_number(moose_var_ref.sys().number(), moose_var_ref.number(), i);
        mfem_true_local_dofs[count] = temp_solution_vector(dof);
        count++;
      }
    }
    pgf.SetFromTrueDofs(mfem_true_local_dofs);
  }
  moose_var_ref.sys().solution().close();
  moose_var_ref.sys().update();
}

void
MFEMProblem::setMOOSEVarData(std::string var_name, EquationSystems & esRef)
{
  auto & moose_var_ref = getVariable(0, var_name);
  MeshBase & libmesh_base = mesh().getMesh();

  unsigned int order = (unsigned int)moose_var_ref.order();

  NumericVector<Number> & temp_solution_vector = moose_var_ref.sys().solution();
  auto & pgf = *(mfem_problem->gridfunctions.Get(var_name));

  const auto * par_fespace = (order > 1) ? pgf.ParFESpace() : nullptr;

  unsigned int count = 0;

  if (moose_var_ref.isNodal())
  {
    mfem::Vector mfem_true_local_dofs(libmesh_base.n_local_nodes());
    mfem_true_local_dofs = *(pgf.GetTrueDofs());

    for (auto & node : as_range(libmesh_base.local_nodes_begin(), libmesh_base.local_nodes_end()))
    {
      unsigned int n_comp = node->n_comp(moose_var_ref.sys().number(), moose_var_ref.number());
      unsigned int node_id = node->id();

      for (unsigned int i = 0; i < n_comp; i++)
      {
        dof_id_type dof = node->dof_number(moose_var_ref.sys().number(), moose_var_ref.number(), i);

        if (order == 1)
        {
          moose_var_ref.sys().solution().set(dof, mfem_true_local_dofs[count]);
        }
        else
        {
          // Local dof # 1:1 correspondance with MOOSE nodes.
          const int mfem_local_dof = mesh().getLocalMFEMNodeId(node_id);

          // No 1:1 correspondance with MOOSE nodes. #local true dofs <= #local dofs.
          // Removes multiple local nodes that refer to same coordinate.
          const int mfem_true_local_dof = par_fespace->GetLocalTDofNumber(mfem_local_dof);

          // Skip dof if it isn't on this processor.
          if (mfem_true_local_dof == -1)
          {
            continue;
          }

          mooseAssert(mfem_true_local_dof >= 0 && mfem_true_local_dof < mfem_true_local_dofs.Size(),
                      "local-true dof out-of-bounds.");

          moose_var_ref.sys().solution().set(dof, mfem_true_local_dofs[mfem_true_local_dof]);
        }

        count++;
      }
    }
  }
  else
  {
    mfem::Vector mfem_local_elems(libmesh_base.n_local_elem());
    mfem_local_elems = *(pgf.GetTrueDofs());

    for (auto & elem :
         as_range(libmesh_base.local_elements_begin(), libmesh_base.local_elements_end()))
    {
      unsigned int n_comp = elem->n_comp(moose_var_ref.sys().number(), moose_var_ref.number());
      unsigned int elem_id = elem->id();
      for (unsigned int i = 0; i < n_comp; i++)
      {
        dof_id_type dof = elem->dof_number(moose_var_ref.sys().number(), moose_var_ref.number(), i);
        moose_var_ref.sys().solution().set(dof, mfem_local_elems[count]);
        count++;
      }
    }
  }
  moose_var_ref.sys().solution().close();
  moose_var_ref.sys().update();
}

InputParameters
MFEMProblem::addMFEMFESpaceFromMOOSEVariable(InputParameters & moosevar_params)
{
  InputParameters mfem_fespace_params(_factory.getValidParams("MFEMFESpace"));
  InputParameters mfem_var_params(_factory.getValidParams("MFEMVariable"));

  const FEFamily var_family =
      Utility::string_to_enum<FEFamily>(moosevar_params.get<MooseEnum>("family"));
  switch (var_family)
  {
    case FEFamily::LAGRANGE:
      mfem_fespace_params.set<MooseEnum>("fespace_type") = std::string("H1");
      break;
    case FEFamily::LAGRANGE_VEC:
      mfem_fespace_params.set<MooseEnum>("fespace_type") = std::string("H1");
      mfem_fespace_params.set<int>("vdim") = 3;
      break;
    case FEFamily::MONOMIAL:
      mfem_fespace_params.set<MooseEnum>("fespace_type") = std::string("L2");
      break;
    case FEFamily::MONOMIAL_VEC:
      mfem_fespace_params.set<MooseEnum>("fespace_type") =
          std::string("L2"); // Create L2 only for now.
      mfem_fespace_params.set<int>("vdim") = 3;
      break;
    default:
      mooseError("Unable to set MFEM FESpace for MOOSE variable");
      break;
  }
  mfem_fespace_params.setParameters<MooseEnum>("order", moosevar_params.get<MooseEnum>("order"));
  int order(moosevar_params.get<MooseEnum>("order"));
  std::string fec_name(
      MFEMFESpace::createFECName(mfem_fespace_params.get<MooseEnum>("fespace_type"), order));
  std::string fespace_name(Utility::enum_to_string(var_family) + "_" + fec_name);

  if (!hasUserObject(fespace_name))
  {
    addFESpace("MFEMFESpace", fespace_name, mfem_fespace_params);
  }

  mfem_var_params.set<UserObjectName>("fespace") = fespace_name;

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
  mooseAssert(ExternalProblem::mesh().type() == "ExclusiveMFEMMesh" ||
                  ExternalProblem::mesh().type() == "CoupledMFEMMesh",
              "Please choose a valid mesh type for an MFEMProblem\n(Either CoupledMFEMMesh or "
              "ExclusiveMFEMMesh)");
  return (ExclusiveMFEMMesh &)_mesh;
}
