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
    _component_x(getCoupledVariable("component_x", 0)),
    _component_y(getCoupledVariable("component_y", 0)),
    _component_z(getCoupledVariable("component_z", 0))
{
  checkVectorComponents();
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

MooseVariable &
VectorVariableToComponentsAux::getCoupledVariable(const std::string & var_name, unsigned int comp)
{
  return getWritableCoupledVariable(var_name, comp);
}

void
VectorVariableToComponentsAux::checkVectorComponents() const
{
  checkVectorComponent(_component_x);
  checkVectorComponent(_component_y);
  checkVectorComponent(_component_z);
}