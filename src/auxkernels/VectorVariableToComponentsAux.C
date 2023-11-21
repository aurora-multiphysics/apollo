#include "VectorVariableToComponentsAux.h"

registerMooseObject("MooseApp", VectorVariableToComponentsAux);

InputParameters
VectorVariableToComponentsAux::validParams()
{
  InputParameters params = WritableVectorAuxKernel::validParams();

  params.addRequiredCoupledVar("component_x", "The x-component of the vector variable.");
  params.addRequiredCoupledVar("component_y", "The y-component of the vector variable.");
  params.addRequiredCoupledVar("component_z", "The z-component of the vector variable.");

  params.addClassDescription("Extract the components of a vector.");

  return params;
}

VectorVariableToComponentsAux::VectorVariableToComponentsAux(const InputParameters & parameters)
  : WritableVectorAuxKernel(parameters),
    _component_x(getWritableCoupledVariable("component_x")),
    _component_y(getWritableCoupledVariable("component_y")),
    _component_z(getWritableCoupledVariable("component_z"))
{
  checkVectorComponents(_component_x, _component_y, _component_z);
}

void
VectorVariableToComponentsAux::compute()
{
  Real value_x = _variable->dofValues()[0];
  Real value_y = _variable->dofValues()[1];
  Real value_z = _variable->dofValues()[2];

  _component_x.setNodalValue(value_x, 0);
  _component_y.setNodalValue(value_y, 0);
  _component_z.setNodalValue(value_z, 0);
}
