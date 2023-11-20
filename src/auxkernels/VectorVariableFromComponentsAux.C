#include "VectorVariableFromComponentsAux.h"

registerMooseObject("MooseApp", VectorVariableFromComponentsAux);

InputParameters
VectorVariableFromComponentsAux::validParams()
{
  InputParameters params = VectorVariableToComponentsAux::validParams();

  params.addClassDescription("Combine three standard variables into a vector variable.");

  return params;
}

VectorVariableFromComponentsAux::VectorVariableFromComponentsAux(const InputParameters & parameters)
  : VectorVariableToComponentsAux(parameters)
{
}

void
VectorVariableFromComponentsAux::compute()
{
  _variable->setDofValue(_component_x.dofValues()[0], 0);
  _variable->setDofValue(_component_y.dofValues()[0], 1);
  _variable->setDofValue(_component_z.dofValues()[0], 2);
}

MooseVariable &
VectorVariableFromComponentsAux::getCoupledVariable(const std::string & var_name, unsigned int comp)
{
  auto * var = getVar(var_name, comp);

  return *var;
}
