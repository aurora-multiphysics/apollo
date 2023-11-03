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

  // Ensure that the variable is a lagrange vector. Later this can be extended
  // to monomials.
  const FEType & the_fetype = _var.feType();
  if (the_fetype.family != LAGRANGE_VEC)
  {
    mooseError("'", _var.name(), "' should be a lagrange vector!");
  }

  // Check components. NB: later we can be smarter and check orders and FE types.
  for (int icomponent = 0; icomponent < 3; icomponent++)
  {
    const VariableValue * the_variable_component = _component_dofs[icomponent];
    if (!the_variable_component)
    {
      mooseError("Component ", icomponent + 1, " is NULL!");
    }
  }
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
