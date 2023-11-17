#include "MultiAppGeneralVectorTransfer.h"
#include "FEProblemBase.h"
#include "SystemBase.h"
#include "VectorVariableFromComponentsAux.h"
#include "VectorVariableToComponentsAux.h"
#include "Moose.h"
#include "AuxiliarySystem.h"
#include "ExecuteMooseObjectWarehouse.h"

/**
 * Register all Moose objects that we would like here.
 */
registerMooseObject("MooseApp", MultiAppGeneralVectorCopyTransfer);
registerMooseObject("MooseApp", MultiAppGeneralVectorNearestLocationTransfer);

template <typename MultiAppTransferClassType>
InputParameters
MultiAppVectorTransferTemplate<MultiAppTransferClassType>::validParams()
{
  InputParameters params = MultiAppTransferClassType::validParams();

  // Combine class description for original class with template class.
  params.addClassDescription(params.getClassDescription() + " (allows vector variables).");

  return params;
}

template <typename MultiAppTransferClassType>
MultiAppVectorTransferTemplate<MultiAppTransferClassType>::MultiAppVectorTransferTemplate(
    const InputParameters & parameters)
  : MultiAppTransferClassType(parameters),
    _has_converted_variables(false),
    _factory(MultiAppTransferClassType::_app.getFactory())
{
  // TODO: - add safety check to ensure template class inherits from correct base.

  // Add all standard component variables.
  convertAllVariables();
}

template <typename MultiAppTransferClassType>
void
MultiAppVectorTransferTemplate<MultiAppTransferClassType>::execute()
{
  // Run through the pre-transfer vector aux-kernels and call the compute() methods. This will
  // ensure that vector components are updated pre-transfer.
  // for (auto * vector_aux_kernel_ptr : _pre_transfer_vector_auxkernels)
  // {
  //   vector_aux_kernel_ptr->compute();
  // }

  // Call execute on inherited class which will work on standard variables.
  MultiAppTransferClassType::execute();

  // Run through the post-transfer vector aux-kernels and call the compute() methods. This will
  // rebuild the vectors from their components.
  // for (auto * vector_aux_kernel_ptr : _post_transfer_vector_auxkernels)
  // {
  //   vector_aux_kernel_ptr->compute();
  // }
}

template <typename MultiAppTransferClassType>
void
MultiAppVectorTransferTemplate<MultiAppTransferClassType>::initialSetup()
{
  // Write methods to check integrity.
  MultiAppTransferClassType::initialSetup();
}

template <typename MultiAppTransferClassType>
std::vector<VariableName>
MultiAppVectorTransferTemplate<MultiAppTransferClassType>::getFromVarNames() const
{
  // return MultiAppTransferClassType::getFromVarNames();
  return _from_var_names_converted;
}

template <typename MultiAppTransferClassType>
std::vector<AuxVariableName>
MultiAppVectorTransferTemplate<MultiAppTransferClassType>::getToVarNames() const
{
  // return MultiAppTransferClassType::getToVarNames();
  return _to_var_names_converted;
}

template <typename MultiAppTransferClassType>
bool
MultiAppVectorTransferTemplate<MultiAppTransferClassType>::isSupportedVectorVariable(
    MooseVariableFEBase & variable) const
{
  const auto family = variable.feType().family;
  const auto order = variable.order();

  return (family == LAGRANGE_VEC || (family == MONOMIAL_VEC && order == CONSTANT));
}

template <typename MultiAppTransferClassType>
bool
MultiAppVectorTransferTemplate<MultiAppTransferClassType>::isSupportedComponentVariable(
    MooseVariableFEBase & variable) const
{
  const auto family = variable.feType().family;
  const auto order = variable.order();

  return (family == LAGRANGE || (family == MONOMIAL && order == CONSTANT));
}

template <typename MultiAppTransferClassType>
bool
MultiAppVectorTransferTemplate<MultiAppTransferClassType>::areCompatibleVariables(
    MooseVariableFEBase & vector_variable, MooseVariableFEBase & component_variable) const
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

template <typename MultiAppTransferClassType>
InputParameters
MultiAppVectorTransferTemplate<MultiAppTransferClassType>::buildInputParametersForComponents(
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

template <typename MultiAppTransferClassType>
std::string
MultiAppVectorTransferTemplate<MultiAppTransferClassType>::buildVectorComponentExtension(
    VectorComponent component) const
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

template <typename MultiAppTransferClassType>
FEProblemBase &
MultiAppVectorTransferTemplate<MultiAppTransferClassType>::getFromProblem() const
{
  if (MultiAppTransferClassType::_current_direction == MultiAppTransferClassType::TO_MULTIAPP)
  {
    return MultiAppTransferClassType::getToMultiApp()->problemBase();
  }
  else if (MultiAppTransferClassType::_current_direction ==
           MultiAppTransferClassType::FROM_MULTIAPP)
  {
    for (int iApp = 0; iApp < MultiAppTransferClassType::getFromMultiApp()->numGlobalApps(); iApp++)
    {
      if (!MultiAppTransferClassType::getFromMultiApp()->hasLocalApp(iApp))
      {
        continue;
      }

      return MultiAppTransferClassType::getFromMultiApp()->appProblemBase(iApp);
    }
  }

  mooseError("The FEProblemBase could not be located.");
}

template <typename MultiAppTransferClassType>
FEProblemBase &
MultiAppVectorTransferTemplate<MultiAppTransferClassType>::getToProblem() const
{
  if (MultiAppTransferClassType::_current_direction == MultiAppTransferClassType::TO_MULTIAPP)
  {
    for (int iApp = 0; iApp < MultiAppTransferClassType::getToMultiApp()->numGlobalApps(); iApp++)
    {
      if (!MultiAppTransferClassType::getToMultiApp()->hasLocalApp(iApp))
      {
        continue;
      }

      return MultiAppTransferClassType::getToMultiApp()->appProblemBase(iApp);
    }
  }
  else if (MultiAppTransferClassType::_current_direction ==
           MultiAppTransferClassType::FROM_MULTIAPP)
  {
    return MultiAppTransferClassType::getFromMultiApp()->problemBase();
  }

  mooseError("The FEProblemBase could not be located.");
}

template <typename MultiAppTransferClassType>
template <class VectorAuxKernelClassType>
VectorAuxKernelClassType &
MultiAppVectorTransferTemplate<MultiAppTransferClassType>::getVectorAuxKernel(
    FEProblemBase & problem, const std::string & name) const
{
  auto & auxiliary_system = problem.getAuxiliarySystem();

  // The auxiliary system will store the VectorAuxKernel derived auxkernels in the
  // nodal_vec_aux_storage or the _elemental_vec_aux_storage.
  auto & nodal_vector_aux_warehouse = auxiliary_system.nodalVectorAuxWarehouse();

  // auto & elemental_vector_aux_warehouse = auxiliary_system.getElementalVectorAuxWarehouse();

  // Attempt to locate.
  auto object_shared_ptr = nodal_vector_aux_warehouse.getObject(name);
  if (!object_shared_ptr)
  {
    mooseError("Failed to find VectorAuxKernel derived object with name '", name, "'.");
  }

  // Now attempt to cast.
  VectorAuxKernelClassType * aux_kernel =
      dynamic_cast<VectorAuxKernelClassType *>(object_shared_ptr.get());
  if (aux_kernel)
  {
    return *aux_kernel; // Reference.
  }
  else
  {
    mooseError("Failed to cast from VectorAuxKernel to derived class.");
  }
}

template <typename MultiAppTransferClassType>
void
MultiAppVectorTransferTemplate<MultiAppTransferClassType>::addVectorAuxKernel(
    FEProblemBase & problem, std::string & vector_name, VectorAuxKernelType type)
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
  else if (isPullTransfer()) // MARK - works!
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

template <typename MultiAppTransferClassType>
void
MultiAppVectorTransferTemplate<MultiAppTransferClassType>::buildVectorComponents(
    FEProblemBase & problem, MooseVariableFEBase & vector_variable)
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

template <typename MultiAppTransferClassType>
MooseVariableFEBase &
MultiAppVectorTransferTemplate<MultiAppTransferClassType>::getVariable(
    FEProblemBase & problem, std::string & variable_name, Moose::VarFieldType type) const
{
  return problem.getVariable(0, variable_name, Moose::VarKindType::VAR_ANY, type);
}

template <typename MultiAppTransferClassType>
MooseVariableFEBase &
MultiAppVectorTransferTemplate<MultiAppTransferClassType>::getStandardVariable(
    FEProblemBase & problem, std::string & variable_name) const
{
  return getVariable(problem, variable_name, Moose::VAR_FIELD_STANDARD);
}

template <typename MultiAppTransferClassType>
MooseVariableFEBase &
MultiAppVectorTransferTemplate<MultiAppTransferClassType>::getVectorVariable(
    FEProblemBase & problem, std::string & variable_name) const
{
  return getVariable(problem, variable_name, Moose::VarFieldType::VAR_FIELD_VECTOR);
}

template <typename MultiAppTransferClassType>
void
MultiAppVectorTransferTemplate<MultiAppTransferClassType>::convertAllVariables()
{
  // Check if this method has been called before.
  if (_has_converted_variables)
  {
    return;
  }

  _has_converted_variables = true;

  // Extract the to/from variable names from the base class.
  std::vector<VariableName> from_var_names = MultiAppTransferClassType::getFromVarNames();
  std::vector<AuxVariableName> to_var_names = MultiAppTransferClassType::getToVarNames();

  // Locate any vector variables. For these variables, create the corrresponding standard
  // variables for their components. Return updated vector strings which contain the new
  // standard variables but exclude the vector variables.
  FEProblemBase & from_problem = getFromProblem();
  FEProblemBase & to_problem = getToProblem();

  _to_var_names_converted = convertVariables<AuxVariableName>(to_problem, to_var_names);
  _from_var_names_converted = convertVariables<VariableName>(from_problem, from_var_names);

  for (auto var_name : _from_var_names_converted)
  {
    std::cout << var_name << std::endl;
  }

  for (auto var_name : _to_var_names_converted)
  {
    std::cout << var_name << std::endl;
  }
}

template <typename MultiAppTransferClassType>
bool
MultiAppVectorTransferTemplate<MultiAppTransferClassType>::isPushTransfer() const
{
  return (MultiAppTransferClassType::_current_direction == MultiAppTransferClassType::TO_MULTIAPP);
}

template <typename MultiAppTransferClassType>
bool
MultiAppVectorTransferTemplate<MultiAppTransferClassType>::isPullTransfer() const
{
  return (MultiAppTransferClassType::_current_direction ==
          MultiAppTransferClassType::FROM_MULTIAPP);
}

template <typename MultiAppTransferClassType>
bool
MultiAppVectorTransferTemplate<MultiAppTransferClassType>::isSupportedTransfer() const
{
  return (isPushTransfer() || isPullTransfer());
}

template <typename MultiAppTransferClassType>
template <class VariableNameClassType>
std::vector<VariableNameClassType>
MultiAppVectorTransferTemplate<MultiAppTransferClassType>::convertVariables(
    FEProblemBase & problem, std::vector<VariableNameClassType> & input_variable_names)
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