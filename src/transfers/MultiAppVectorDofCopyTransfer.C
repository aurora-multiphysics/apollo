#include "MultiAppVectorDofCopyTransfer.h"
#include "FEProblemBase.h"
#include "MultiApp.h"
#include "SystemBase.h"
#include "MooseError.h"

#include "libmesh/string_to_enum.h"
#include "libmesh/dof_object.h"
#include "libmesh/id_types.h"

// NB: - do not include "registerMooseObject". This class should not be used in the input file.

InputParameters
MultiAppVectorDofCopyTransfer::validParams()
{
  InputParameters params = MultiAppFieldTransfer::validParams();

  params.addClassDescription(
      "Base class for copying degrees-of-freedom values (nonlinear and auxiliary) between apps "
      "that have identical meshes.");

  return params;
}

MultiAppVectorDofCopyTransfer::MultiAppVectorDofCopyTransfer(const InputParameters & parameters)
  : MultiAppFieldTransfer(parameters)
{
}

void
MultiAppVectorDofCopyTransfer::transfer(FEProblemBase & to_problem, FEProblemBase & from_problem)
{
  // Check variables were supplied and number of variables match.
  if (getFromVarNames().empty())
  {
    mooseError("No source variable names were specified.");
  }

  if (getToVarNames().empty())
  {
    mooseError("No destination variable names were specified.");
  }

  if (getFromVarNames().size() != getToVarNames().size())
  {
    mooseError("Number of variables transferred must be the same in both systems.");
  }

  // Check variables actually exist.
  for (auto & to_variable : getToVarNames())
  {
    checkVariable(to_problem, to_variable);
  }

  for (auto & from_variable : getFromVarNames())
  {
    checkVariable(from_problem, from_variable);
  }

  // Get meshes.
  MeshBase & from_mesh = from_problem.mesh().getMesh();
  MeshBase & to_mesh = to_problem.mesh().getMesh();

  if ((from_mesh.n_nodes() != to_mesh.n_nodes()) || (from_mesh.n_elem() != to_mesh.n_elem()))
  {
    mooseError("The meshes must be identical.");
  }

  // Iterate over variable pairs.
  for (int ivariable = 0; ivariable < getToVarNames().size(); ivariable++)
  {
    // Check variable types match. For this initial version, we are only going to support
    // LagrangeVec and MonomialVec vector variables.
    const VariableName from_variable_name = getFromVarNames()[ivariable];
    const AuxVariableName to_variable_name = getToVarNames()[ivariable];

    // NB: for this initial version, only allow vector variables.
    MooseVariableFEBase & from_variable = from_problem.getVariable(
        0, from_variable_name, Moose::VarKindType::VAR_ANY, Moose::VarFieldType::VAR_FIELD_VECTOR);

    MooseVariableFEBase & to_variable = to_problem.getVariable(
        0, to_variable_name, Moose::VarKindType::VAR_ANY, Moose::VarFieldType::VAR_FIELD_VECTOR);

    // Check FE types match (same order and family).
    if (from_variable.feType() != to_variable.feType())
    {
      mooseError("The vector variables must be of the same order and family.");
    }

    // Check it is either a LagrangeVec or MonomialVec.
    if (!isSupportedVectorVariable(from_variable) || !isSupportedVectorVariable(to_variable))
    {
      mooseError("Only LagrangeVec and constant order MonomialVec vectors are supported.");
    }

    // Get solutions.
    auto & from_solution = from_variable.sys().solution();
    auto & to_solution = to_variable.sys().solution();

    // Transfer node dofs.
    for (auto * to_node : as_range(to_mesh.local_nodes_begin(), to_mesh.local_nodes_end()))
    {
      auto * from_node = from_mesh.node_ptr(to_node->id());

      transferVectorDofs(
          *to_node, *from_node, to_variable, from_variable, to_solution, from_solution);
    }

    // Transfer element dofs.
    for (auto * to_element : as_range(to_mesh.local_elements_begin(), to_mesh.elements_end()))
    {
      auto * from_element = from_mesh.elem_ptr(to_element->id());

      transferVectorDofs(
          *to_element, *from_element, to_variable, from_variable, to_solution, from_solution);
    }

    // Update.
    to_solution.close();
    to_variable.sys().update();
  }
}

void
MultiAppVectorDofCopyTransfer::transferVectorDofs(libMesh::DofObject & to_object,
                                                  libMesh::DofObject & from_object,
                                                  MooseVariableFieldBase & to_var,
                                                  MooseVariableFieldBase & from_var,
                                                  NumericVector<Number> & to_solution,
                                                  NumericVector<Number> & from_solution)
{
  const auto num_components_to = to_var.count();
  const auto num_components_from = from_var.count();

  if (num_components_to != 1 || num_components_from != 1)
  {
    mooseError("Vector variables should only have a single component.");
  }

  // Find the number of dofs at the node for the from and to variables.
  const auto num_to_var_dofs = to_object.n_dofs(to_var.sys().number(), to_var.number());
  const auto num_from_var_dofs = from_object.n_dofs(from_var.sys().number(), from_var.number());

  if (num_to_var_dofs != num_from_var_dofs)
  {
    mooseError("The variables should have the same number of dofs at each node.");
  }

  if (num_to_var_dofs == 0) // Nothing to do here.
  {
    return;
  }

  // Iterate over dofs at the node.
  for (int icomponent = 0; icomponent < num_to_var_dofs; icomponent++)
  {
    dof_id_type to_dof = to_object.dof_number(to_var.sys().number(), to_var.number(), icomponent);
    dof_id_type from_dof =
        from_object.dof_number(from_var.sys().number(), from_var.number(), icomponent);

    Real from_value = from_solution(from_dof);
    to_solution.set(to_dof, from_value);
  }
}

bool
MultiAppVectorDofCopyTransfer::isSupportedVectorVariable(MooseVariableFEBase & variable) const
{
  const auto family = variable.feType().family;
  const auto order = variable.order();

  return (family == LAGRANGE_VEC || (family == MONOMIAL_VEC && order == CONSTANT));
}