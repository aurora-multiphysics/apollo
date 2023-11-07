#include "VectorVariableFromComponentsAux.h"

registerMooseObject("MooseApp", VectorVariableFromComponentsAux);

InputParameters
VectorVariableFromComponentsAux::validParams()
{
  InputParameters params = VectorAuxKernel::validParams();

  params.addRequiredCoupledVar("component_x", "The x-component of the vector variable.");
  params.addRequiredCoupledVar("component_y", "The y-component of the vector variable.");
  params.addRequiredCoupledVar("component_z", "The z-component of the vector variable.");

  params.addClassDescription("Combines three standard variables into a vector variable.");

  return params;
}

VectorVariableFromComponentsAux::VectorVariableFromComponentsAux(const InputParameters & parameters)
  : VectorAuxKernel(parameters),
    _component_x(coupledValue("component_x")),
    _component_y(coupledValue("component_y")),
    _component_z(coupledValue("component_z"))
{
  // Sanity check 1: the variable we are constructing from the components must be
  // a lagrange vector or a monomial vector.
  const auto the_vector_family = _var.feType().family;
  const auto the_vector_order = _var.feType().order.get_order();

  if (the_vector_family != LAGRANGE_VEC && the_vector_family != MONOMIAL_VEC)
  {
    mooseError("Only Lagrange vectors and Monomial vectors are supported.");
  }

  // Sanity check 2: if we have a monomial vector we only support constant order for now.
  if (the_vector_family == MONOMIAL_VEC && the_vector_order != Order::CONSTANT)
  {
    mooseError("Monomial vectors are only supported to constant order.");
  }

  // Sanity check 3: Ensure that component variables are of the correct family and same
  // order as the vector.
  {
  }
}

void
VectorVariableFromComponentsAux::compute()
{
  _variable->setDofValue(_component_x[0], 0);
  _variable->setDofValue(_component_y[0], 1);
  _variable->setDofValue(_component_z[0], 2);
}
