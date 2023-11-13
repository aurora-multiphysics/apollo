#pragma once

#include "MultiAppVectorCopyTransfer.h"

registerMooseObject("MooseApp", MultiAppVectorCopyTransfer);

InputParameters
MultiAppVectorCopyTransfer::validParams()
{
  InputParameters params = MultiAppVectorDofCopyTransfer::validParams();

  params.addRequiredParam<std::vector<AuxVariableName>>(
      "variable", "The auxiliary vector variables to store the transferred values in.");

  params.addRequiredParam<std::vector<VariableName>>("source_variable",
                                                     "The vector variables to transfer from.");

  params.addClassDescription(
      "Copies vector variables between multiapps that have identical meshes.");

  return params;
}

MultiAppVectorCopyTransfer::MultiAppVectorCopyTransfer(const InputParameters & parameters)
  : MultiAppVectorDofCopyTransfer(parameters),
    _from_var_names(getParam<std::vector<VariableName>>("source_variable")),
    _to_var_names(getParam<std::vector<AuxVariableName>>("variable"))
{
}

void
MultiAppVectorCopyTransfer::execute()
{
  if (_current_direction == TO_MULTIAPP)
  {
    FEProblemBase & from_problem = getToMultiApp()->problemBase();

    for (int iApp = 0; iApp < getToMultiApp()->numGlobalApps(); iApp++)
    {
      if (!getToMultiApp()->hasLocalApp(iApp))
      {
        continue;
      }

      FEProblemBase & to_problem = getToMultiApp()->appProblemBase(iApp);

      transfer(to_problem, from_problem);
    }
  }
  else if (_current_direction == FROM_MULTIAPP)
  {
    FEProblemBase & to_problem = getFromMultiApp()->problemBase();

    for (int iApp = 0; iApp < getFromMultiApp()->numGlobalApps(); iApp++)
    {
      if (!getFromMultiApp()->hasLocalApp(iApp))
      {
        continue;
      }

      FEProblemBase & from_problem = getFromMultiApp()->appProblemBase(iApp);

      transfer(to_problem, from_problem);
    }
  }
  else
  {
    mooseError("The transfer direction has not been implemented.");
  }
}