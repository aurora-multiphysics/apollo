#include "VectorVariableComponentsAux.h"

registerMooseObject("MooseApp", VectorVariableComponentsAux);

InputParameters
VectorVariableComponentsAux::validParams()
{
  InputParameters params = AuxKernel::validParams();

  params.addRequiredCoupledVar("vector_variable", "The vector variable from which to extract the components.");
  params.addRequiredCoupledVar("component_x", "The x-component of the vector variable.");
  params.addRequiredCoupledVar("component_y", "The y-component of the vector variable.");
  params.addRequiredCoupledVar("component_z", "The z-component of the vector variable.");

  params.addClassDescription("Extract the components of a vector.");

  return params;
}

VectorVariableComponentsAux::VectorVariableComponentsAux(const InputParameters & parameters)
  : AuxKernel(parameters)
{
}

void
VectorVariableComponentsAux::compute()
{
  auto & coupled_vector_value = coupledVectorValue("vector_variable");

  RealVectorValue values = coupled_vector_value[0];
  Real value_x = values(0), value_y = values(1), value_z = values(2);

  auto & component_x = writableVariable("component_x");
  auto & component_y = writableVariable("component_y");
  auto & component_z = writableVariable("component_z");

  component_x.setNodalValue(value_x, 0);
  component_y.setNodalValue(value_y, 0);
  component_z.setNodalValue(value_z, 0);
}
