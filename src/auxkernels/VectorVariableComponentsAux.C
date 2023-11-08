#include "VectorVariableComponentsAux.h"

registerMooseObject("MooseApp", VectorVariableComponentsAux);

InputParameters
VectorVariableComponentsAux::validParams()
{
  InputParameters params = VectorAuxKernel::validParams();

  params.addRequiredCoupledVar("component_x", "The x-component of the vector variable.");
  params.addRequiredCoupledVar("component_y", "The y-component of the vector variable.");
  params.addRequiredCoupledVar("component_z", "The z-component of the vector variable.");

  params.addClassDescription("Extract the components of a vector.");

  return params;
}

VectorVariableComponentsAux::VectorVariableComponentsAux(const InputParameters & parameters)
  : VectorAuxKernel(parameters),
    _component_x(writableVariable("component_x")),
    _component_y(writableVariable("component_y")),
    _component_z(writableVariable("component_z"))
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

MooseVariable &
VectorVariableComponentsAux::writableVariable(const std::string & var_name, unsigned int comp)
{
  auto * var = dynamic_cast<MooseVariable *>(getVar(var_name, comp));

  // Make sure only one object can access a variable.
  checkWritableVar(var);

  return *var;
}
