#include "VectorVariableFromComponentsAux.h"

registerMooseObject("MooseApp", VectorVariableFromComponentsAux);

InputParameters
VectorVariableFromComponentsAux::validParams()
{
  InputParameters params = VectorAuxKernel::validParams();
  params.addCoupledVar("component_variables",
                       "The variables that make up each component of the output vector variable.");
  params.addClassDescription("Combines three standard variables into a vector variable.");

  return params;
}

VectorVariableFromComponentsAux::VectorVariableFromComponentsAux(const InputParameters & parameters)
  : VectorAuxKernel(parameters), _component_dofs(coupledAllDofValues("component_variables"))
{
  // Check the number of component variables is equal to 3.
  if (_component_dofs.size() != 3)
  {
    paramError("variable",
               "component variables should have 3 components, but ",
               _component_dofs.size(),
               " component variables were specified.");
  }

  // TODO: - ensure that the FE families are appropriate. i.e. LAGRANGE if _vec is LAGRANGE_vec
  // TODO: - ensure that the orders match.
}

void
VectorVariableFromComponentsAux::compute()
{
  _local_sol.resize(3);

  for (int icomponent = 0; icomponent < 3; icomponent++)
  {
    _local_sol(icomponent) = (*_component_dofs[icomponent])[0];
  }

  _var.setDofValues(_local_sol);
}
