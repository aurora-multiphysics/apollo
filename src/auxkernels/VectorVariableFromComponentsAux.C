#include "VectorVariableFromComponentsAux.h"

registerMooseObject("MooseApp", VectorVariableFromComponentsAux);

InputParameters
VectorVariableFromComponentsAux::validParams()
{
  InputParameters params = ApolloVectorAuxKernel::validParams();

  params.addRequiredCoupledVar("component_x", "The x-component of the vector variable.");
  params.addRequiredCoupledVar("component_y", "The y-component of the vector variable.");
  params.addRequiredCoupledVar("component_z", "The z-component of the vector variable.");

  params.addClassDescription("Combines three standard variables into a vector variable.");

  return params;
}

VectorVariableFromComponentsAux::VectorVariableFromComponentsAux(const InputParameters & parameters)
  : ApolloVectorAuxKernel(parameters),
    _component_x(coupledValue("component_x")),
    _component_y(coupledValue("component_y")),
    _component_z(coupledValue("component_z")),
    _vector_order(_var.order()),
    _vector_family(_var.feType().family)
{
  checkVectorVariable();
  checkVectorComponents();
}

void
VectorVariableFromComponentsAux::compute()
{
  _variable->setDofValue(_component_x[0], 0);
  _variable->setDofValue(_component_y[0], 1);
  _variable->setDofValue(_component_z[0], 2);
}

void
VectorVariableFromComponentsAux::checkVectorVariable() const
{
  if (_vector_family != LAGRANGE_VEC && _vector_family != MONOMIAL_VEC)
  {
    mooseError("Only Lagrange and Monomial vectors are supported.");
  }

  if (_vector_family == MONOMIAL_VEC && _vector_order != CONSTANT)
  {
    mooseError("Monomial vectors are supported only for constant order.");
  }
}

void
VectorVariableFromComponentsAux::checkVectorComponents() const
{
  auto & component_variables = getCoupledStandardMooseVars();

  if (component_variables.size() != 3)
  {
    mooseError("There are ", component_variables.size(), " component variables. Expected 3.");
  }

  for (auto component_variable : component_variables)
  {
    if (!component_variable)
    {
      mooseError("The component variable was NULL.");
    }

    checkVectorComponent(*component_variable);
  }
}

void
VectorVariableFromComponentsAux::checkVectorComponent(
    const MooseVariable & component_variable) const
{
  auto component_order = component_variable.order();
  auto component_family = component_variable.feType().family;

  bool correct_family = ((component_family == LAGRANGE && _vector_family == LAGRANGE_VEC) ||
                         (component_family == MONOMIAL && _vector_family == MONOMIAL_VEC));
  if (!correct_family)
  {
    mooseError("Component '", component_variable.name(), "' is of the incorrect family.");
  }

  bool correct_order = (component_order == _vector_order);
  if (!correct_order)
  {
    mooseError("Component '", component_variable.name(), "' has a different order to the vector.");
  }
}