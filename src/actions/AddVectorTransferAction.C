#include "AddVectorTransferAction.h"
#include "FEProblem.h"

registerMooseAction("MooseApp", AddVectorTransferAction, "add_vector_transfer");

InputParameters
AddVectorTransferAction::validParams()
{
  InputParameters params = MooseObjectAction::validParams();
  params.addClassDescription("Add a Transfer object that can handle vectors to the simulation.");
  return params;
}

AddVectorTransferAction::AddVectorTransferAction(const InputParameters & params)
  : MooseObjectAction(params)
{
}

void
AddVectorTransferAction::act()
{
  /**
   * Okay, we have the MOOSE object parameters. This is good! And we have already created the
   * parameters for the transfer type that the user has specified. We will need to modify some of
   * these input parameters.
   */
  InputParameters & input_params = getObjectParams();

  std::cout << "from_problem = ";
  for (auto & var_name : getFromProblem().getVariableNames())
  {
    std::cout << var_name << ", ";
  }

  std::cout << std::endl;

  std::cout << "to_problem = ";
  for (auto & var_name : getToProblem().getVariableNames())
  {
    std::cout << var_name << ", ";
  }

  std::cout << std::endl;

  std::cout << "is push transfer? " << (isPushTransfer() ? "YES" : "NO") << std::endl;
  std::cout << "is pull transfer? " << (isPullTransfer() ? "YES" : "NO") << std::endl;

  convertAllVariables();
  std::cout << "finished converting all variables..." << std::endl;

  // TODO: - set input variable names here

  // TODO: - set "source_types" here if required.

  // Modify source variables and variables parameters here...

  // TODO: - add transfer here...
  // _problem->addTransfer(_type, _name, input_params);
}

const std::shared_ptr<MultiApp>
AddVectorTransferAction::getFromMultiApp() const
{
  if (getObjectParams().isParamValid("from_multi_app"))
  {
    return _problem->getMultiApp(getObjectParams().get<MultiAppName>("from_multi_app"));
  }

  return nullptr;
}

const std::shared_ptr<MultiApp>
AddVectorTransferAction::getToMultiApp() const
{
  if (getObjectParams().isParamValid("to_multi_app"))
  {
    return _problem->getMultiApp(getObjectParams().get<MultiAppName>("to_multi_app"));
  }

  return nullptr;
}

const std::vector<VariableName> &
AddVectorTransferAction::getFromVarNames() const
{
  return getObjectParams().get<std::vector<VariableName>>("source_variable");
}

const std::vector<AuxVariableName> &
AddVectorTransferAction::getToVarNames() const
{
  return getObjectParams().get<std::vector<AuxVariableName>>("variable");
}

FEProblemBase &
AddVectorTransferAction::getToProblem() const
{
  if (isPullTransfer()) // subapp (FROM) --> master.
  {
    auto & subapp = getFromMultiApp();

    return subapp->problemBase();
  }
  else if (isPushTransfer()) // master (FROM) --> subapp
  {
    auto & subapp = getToMultiApp();

    for (int iapp = 0; iapp < subapp->numGlobalApps(); iapp++)
    {
      if (subapp->hasLocalApp(iapp))
      {
        return subapp->appProblemBase(iapp);
      }
    }
  }

  mooseError("The FEProblemBase could not be located.");
}

// TODO: - why is it this way around?
FEProblemBase &
AddVectorTransferAction::getFromProblem() const
{
  if (isPullTransfer()) // subapp --> master (TO).
  {
    auto & subapp = getFromMultiApp();

    for (int iapp = 0; iapp < subapp->numGlobalApps(); iapp++)
    {
      if (subapp->hasLocalApp(iapp))
      {
        return subapp->appProblemBase(iapp);
      }
    }
  }
  else if (isPushTransfer()) // master --> subapp (TO).
  {
    auto & subapp = getToMultiApp();

    return subapp->problemBase();
  }

  mooseError("The FEProblemBase could not be located.");
}

void
AddVectorTransferAction::addVectorAuxKernel(FEProblemBase & problem,
                                            std::string & vector_name,
                                            VectorAuxKernelType type)
{
  std::string aux_kernel_name;
  std::string unique_aux_kernel_name;

  switch (type)
  {
    case VectorAuxKernelType::RECOVER_VECTOR_POST_TRANSFER:
      aux_kernel_name = "VectorVariableFromComponentsAux";
      unique_aux_kernel_name = vector_name + "_from_components";
      break;
    case VectorAuxKernelType::PREPARE_VECTOR_FOR_TRANSFER:
      aux_kernel_name = "VectorVariableToComponentsAux";
      unique_aux_kernel_name = vector_name + "_to_components";
      break;
    default:
      mooseError("Unsupported vector auxkernel type.");
      break;
  }

  auto params = _factory.getValidParams(aux_kernel_name);

  params.set<AuxVariableName>("variable") = vector_name;
  params.set<std::vector<VariableName>>("component_x") = {
      buildVectorComponentName(vector_name, VectorComponent::X)};
  params.set<std::vector<VariableName>>("component_y") = {
      buildVectorComponentName(vector_name, VectorComponent::Y)};
  params.set<std::vector<VariableName>>("component_z") = {
      buildVectorComponentName(vector_name, VectorComponent::Z)};

  /**
   * Note on execution times:
   *
   * Case 1: "Push problem" --> we need to execute just before we start running the subapp.
   * Case 2: "Pull problem" --> we need to execute when we've finished running the subapp.
   */
  if (isPushTransfer())
  {
    params.set<ExecFlagEnum>("execute_on") = "timestep_begin MULTIAPP_FIXED_POINT_BEGIN";
  }
  else if (isPullTransfer())
  {
    params.set<ExecFlagEnum>("execute_on") = "timestep_end";
  }
  else
  {
    mooseError("Unsupported transfer direction specified.");
  }

  problem.addAuxKernel(aux_kernel_name, unique_aux_kernel_name, params);

  // Locate auxkernel we've just added and add a pointer to the appropriate array. This will be
  // used to call the execute method.
  // if (type == VectorAuxKernelType::RECOVER_VECTOR_POST_TRANSFER)
  // {
  //   auto & vector_aux_kernel =
  //       getVectorAuxKernel<VectorVariableFromComponentsAux>(problem, unique_aux_kernel_name);

  //   _post_transfer_vector_auxkernels.push_back(&vector_aux_kernel);
  // }
  // else
  // {
  //   auto & vector_aux_kernel =
  //       getVectorAuxKernel<VectorVariableToComponentsAux>(problem, unique_aux_kernel_name);

  //   _pre_transfer_vector_auxkernels.push_back(&vector_aux_kernel);
  // }
}

void
AddVectorTransferAction::convertAllVariables()
{
  // Check if this method has been called before.
  if (_has_converted_variables)
  {
    return;
  }

  _has_converted_variables = true;

  // Extract the to/from variable names from the base class.
  std::vector<VariableName> from_var_names = getFromVarNames();
  std::vector<AuxVariableName> to_var_names = getToVarNames();

  // Locate any vector variables. For these variables, create the corrresponding standard
  // variables for their components. Return updated vector strings which contain the new
  // standard variables but exclude the vector variables.
  FEProblemBase & from_problem = getFromProblem();
  FEProblemBase & to_problem = getToProblem();

  _to_var_names_converted = convertVariables<AuxVariableName>(to_problem, to_var_names);
  _from_var_names_converted = convertVariables<VariableName>(from_problem, from_var_names);

  /**
   * Mark: - Testing...
   */
  std::cout << "from_var_names_converted: " << std::endl;
  for (auto var_name : _from_var_names_converted)
  {
    std::cout << var_name << std::endl;
  }

  std::cout << "to_var_names_converted: " << std::endl;
  for (auto var_name : _to_var_names_converted)
  {
    std::cout << var_name << std::endl;
  }
}

template <class VariableNameClassType>
std::vector<VariableNameClassType>
AddVectorTransferAction::convertVariables(FEProblemBase & problem,
                                          std::vector<VariableNameClassType> & input_variable_names)
{
  std::vector<VariableNameClassType> output_variable_names;

  for (auto & variable_name : input_variable_names)
  {
    MooseVariableFEBase & variable = getVariable(problem, variable_name);

    if (!isSupportedVectorVariable(variable))
    {
      // Nothing to do here. Pushback variable.
      output_variable_names.push_back(variable_name);
      continue;
    }

    // Create corresponding standard variables for components. Add to output array.
    buildVectorComponents(problem, variable);

    auto component_x_name = buildVectorComponentName(variable_name, VectorComponent::X);
    auto component_y_name = buildVectorComponentName(variable_name, VectorComponent::Y);
    auto component_z_name = buildVectorComponentName(variable_name, VectorComponent::Z);

    output_variable_names.push_back(component_x_name);
    output_variable_names.push_back(component_y_name);
    output_variable_names.push_back(component_z_name);

    /**
     * Case 1: "from problem" --> Update vector components. These will be transferred.
     * Case 2: "to problem" --> Received vector components. Rebuild vector from components.
     */
    if (&problem == &getFromProblem())
    {
      // Add vector to set for checking later.
      _vector_source_names.insert(variable_name);

      addVectorAuxKernel(problem, variable_name, VectorAuxKernelType::PREPARE_VECTOR_FOR_TRANSFER);
    }
    else if (&problem == &getToProblem())
    {
      addVectorAuxKernel(problem, variable_name, VectorAuxKernelType::RECOVER_VECTOR_POST_TRANSFER);
    }
    else
    {
      mooseError("Unknown FEProblemBase.");
    }
  }

  output_variable_names.shrink_to_fit();

  return output_variable_names;
}

void
AddVectorTransferAction::buildVectorComponents(FEProblemBase & problem,
                                               MooseVariableFEBase & vector_variable)
{
  // Safety check.
  if (!isSupportedVectorVariable(vector_variable))
  {
    mooseError("The variable '", vector_variable.name(), "' is not a supported vector variable.");
  }

  // Iterate over all components.
  for (auto component : getAllComponents())
  {
    // Create vector variable component name.
    std::string component_name = buildVectorComponentName(vector_variable.name(), component);

    // Does the component exist? If so, is it of the correct type. If it is not compatible then we
    // have a problem.
    bool component_already_exists = problem.hasScalarVariable(component_name);

    if (problem.hasScalarVariable(component_name))
    {
      // Component already exists! Check that it is compatible with the vector variable and emit a
      // warning that the variable already exists so we don't blindly overwrite the user's values.
      MooseVariableFEBase & component_variable = getVariable(problem, component_name);

      if (areCompatibleVariables(vector_variable, component_variable))
      {
        mooseWarning("Found existing variable '",
                     component_name,
                     "' which is compatible with vector '",
                     vector_variable.name(),
                     "'.");
      }
      else
      {
        mooseError("Found existing variable '",
                   component_name,
                   "' which is incompatible with vector '",
                   vector_variable.name(),
                   "'.");
      }
    }
    else
    {
      auto component_parameters = buildInputParametersForComponents(vector_variable);

      // Add to system.
      problem.addAuxVariable("MooseVariable", component_name, component_parameters);
    }
  }
}

std::string
AddVectorTransferAction::buildVectorComponentExtension(VectorComponent component) const
{
  std::string extension;

  switch (component)
  {
    case VectorComponent::X:
      extension = "_x";
      break;
    case VectorComponent::Y:
      extension = "_y";
      break;
    case VectorComponent::Z:
      extension = "_z";
      break;
    default:
      mooseError("An unsupported vector component was detected.");
      break;
  }

  return extension;
}

InputParameters
AddVectorTransferAction::buildInputParametersForComponents(
    MooseVariableFEBase & vector_variable) const
{
  if (!isSupportedVectorVariable(vector_variable))
  {
    mooseError("'", vector_variable.name(), "' is not a supported vector variable.");
  }

  const FEType & vector_type = vector_variable.feType();

  InputParameters params = _factory.getValidParams("MooseVariable");

  // Should be same order as vector variable but obviously of a different family.
  params.set<MooseEnum>("order") = vector_type.order.get_order();
  params.set<MooseEnum>("family") = vector_type.family == LAGRANGE_VEC ? "LAGRANGE" : "MONOMIAL";

  return params;
}

bool
AddVectorTransferAction::isSupportedVectorVariable(MooseVariableFEBase & variable) const
{
  const auto family = variable.feType().family;
  const auto order = variable.order();

  return (family == LAGRANGE_VEC || (family == MONOMIAL_VEC && order == CONSTANT));
}

bool
AddVectorTransferAction::isSupportedComponentVariable(MooseVariableFEBase & variable) const
{
  const auto family = variable.feType().family;
  const auto order = variable.order();

  return (family == LAGRANGE || (family == MONOMIAL && order == CONSTANT));
}

bool
AddVectorTransferAction::areCompatibleVariables(MooseVariableFEBase & vector_variable,
                                                MooseVariableFEBase & component_variable) const
{
  bool supported_vector = isSupportedVectorVariable(vector_variable);
  bool supported_component = isSupportedComponentVariable(component_variable);

  if (!supported_vector || !supported_component)
  {
    return false;
  }

  const auto component_family = component_variable.feType().family;
  const auto component_order = component_variable.order();

  const auto vector_family = vector_variable.feType().family;
  const auto vector_order = vector_variable.order();

  bool compatibleFamilies = (component_family == LAGRANGE && vector_family == LAGRANGE_VEC) ||
                            (component_family == MONOMIAL && vector_family == MONOMIAL_VEC);

  bool compatibleOrders = (component_order == vector_order);

  return (compatibleFamilies && compatibleOrders);
}

bool
AddVectorTransferAction::isPushTransfer() const
{
  return getObjectParams().isParamValid("to_multi_app");
}

bool
AddVectorTransferAction::isPullTransfer() const
{
  return getObjectParams().isParamValid("from_multi_app");
}

bool
AddVectorTransferAction::isSupportedTransfer() const
{
  return (isPushTransfer() || isPullTransfer());
}

MooseVariableFEBase &
AddVectorTransferAction::getVariable(FEProblemBase & problem,
                                     std::string & variable_name,
                                     Moose::VarFieldType type) const
{
  return problem.getVariable(0, variable_name, Moose::VarKindType::VAR_ANY, type);
}

MooseVariableFEBase &
AddVectorTransferAction::getStandardVariable(FEProblemBase & problem,
                                             std::string & variable_name) const
{
  return getVariable(problem, variable_name, Moose::VAR_FIELD_STANDARD);
}

MooseVariableFEBase &
AddVectorTransferAction::getVectorVariable(FEProblemBase & problem,
                                           std::string & variable_name) const
{
  return getVariable(problem, variable_name, Moose::VarFieldType::VAR_FIELD_VECTOR);
}
