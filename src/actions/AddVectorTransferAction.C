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
