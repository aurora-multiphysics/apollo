#include "MFEMAuxSolver.h"
#include "Function.h"

registerMooseObject("ApolloApp", MFEMAuxSolver);

InputParameters
MFEMAuxSolver::validParams()
{
  InputParameters params = GeneralUserObject::validParams();

  params.registerBase("AuxKernel");

  return params;
}

MFEMAuxSolver::MFEMAuxSolver(const InputParameters & parameters) : GeneralUserObject(parameters) {}

hephaestus::AuxSolver *
MFEMAuxSolver::getAuxSolver()
{
  return _auxsolver;
}

MFEMAuxSolver::~MFEMAuxSolver() {}
