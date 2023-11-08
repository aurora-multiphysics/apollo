#include "VectorVariableComponentsAux.h"

registerMooseObject("MooseApp", VectorVariableComponentsAux);

InputParameters
VectorVariableComponentsAux::validParams()
{
  InputParameters params = VectorVariableFromComponentsAux::validParams();

  params.addClassDescription("Extract the components of a vector.");

  return params;
}

VectorVariableComponentsAux::VectorVariableComponentsAux(const InputParameters & parameters)
  : VectorVariableFromComponentsAux(parameters)
{
}

void
VectorVariableComponentsAux::compute()
{
  Real value_x = _variable->dofValues()[0];
  Real value_y = _variable->dofValues()[1];
  Real value_z = _variable->dofValues()[2];

  _component_x.setNodalValue(value_x, 0);
  _component_y.setNodalValue(value_y, 0);
  _component_z.setNodalValue(value_z, 0);
}
