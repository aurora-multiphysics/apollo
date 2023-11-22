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
  initializeMultiApps();
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

  // Print source variables.
  // std::cout << "fromVarNames: ";
  // for (auto & source_variable : getFromVarNames())
  // {
  //   std::cout << source_variable << ", ";
  // }
  // std::cout << std::endl;

  // std::cout << "toVarNames: ";
  // for (auto & source_variable : getToVarNames())
  // {
  //   std::cout << source_variable << ", ";
  // }
  // std::cout << std::endl;

  // _problem->addTransfer(_type, _name, input_params);
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

void
AddVectorTransferAction::initializeMultiApps()
{
  mooseError("Not implemented");
}

FEProblemBase &
AddVectorTransferAction::getFromProblem() const
{
  mooseError("Not implemented");
}

FEProblemBase &
AddVectorTransferAction::getToProblem() const
{
  mooseError("Not implemented");
}

const std::shared_ptr<MultiApp>
AddVectorTransferAction::getFromMultiApp() const
{
  if (!_from_multi_app)
  {
    mooseError(
        "A from_multiapp was requested but is unavailable. Check the from_multi_app parameter");
  }

  return _from_multi_app;
}

const std::shared_ptr<MultiApp>
AddVectorTransferAction::getToMultiApp() const
{
  if (!_to_multi_app)
  {
    mooseError("A to_multiapp was requested but is unavailable. Check the to_multi_app parameter");
  }

  return _to_multi_app;
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
