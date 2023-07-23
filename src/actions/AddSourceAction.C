#include "AddSourceAction.h"

registerMooseAction("ApolloApp", AddSourceAction, "add_mfem_source");

InputParameters
AddSourceAction::validParams()
{
  InputParameters params = MooseObjectAction::validParams();
  params.addClassDescription("Add a source term to an MFEM problem solved in the simulation.");
  return params;
}

AddSourceAction::AddSourceAction(const InputParameters & parameters) : MooseObjectAction(parameters)
{
}

void
AddSourceAction::act()
{
  MFEMProblem * mfem_problem = dynamic_cast<MFEMProblem *>(_problem.get());
  if (mfem_problem)
    mfem_problem->addSource(_type, _name, _moose_object_pars);
}
