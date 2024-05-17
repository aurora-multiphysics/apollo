#include "WritableVectorAuxKernel.h"

registerMooseObject("MooseApp", WritableVectorAuxKernel);

WritableVectorAuxKernel::WritableVectorAuxKernel(const InputParameters & parameters)
  : VectorAuxKernel(parameters), _vector_order(_var.order()), _vector_family(_var.feType().family)
{
  checkVectorVariable();
}

const MooseVariable &
WritableVectorAuxKernel::getNonWritableCoupledVariable(const std::string & var_name,
                                                       unsigned int comp)
{
  const MooseVariable * var = getVar(var_name, comp);
  if (!var)
  {
    mooseError("Could not find coupled variable with name '", var_name, "'.");
  }

  return *var;
}

MooseVariable &
WritableVectorAuxKernel::getWritableCoupledVariable(const std::string & var_name, unsigned int comp)
{
  auto * var = getVar(var_name, comp);

  // Make sure only one object can access a variable.
  checkWritableVar(var);

  return *var;
}

void
WritableVectorAuxKernel::checkVectorVariable() const
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
WritableVectorAuxKernel::checkVectorComponent(const MooseVariable & component_variable) const
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

void
WritableVectorAuxKernel::checkVectorComponents(const MooseVariable & component_x,
                                               const MooseVariable & component_y,
                                               const MooseVariable & component_z) const
{
  checkVectorComponent(component_x);
  checkVectorComponent(component_y);
  checkVectorComponent(component_z);
}
