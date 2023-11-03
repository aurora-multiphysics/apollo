#include "AddTransferLagrangeVecAction.h"
#include "FEProblem.h"
#include "Factory.h"
#include "libmesh/string_to_enum.h"
#include "ExternalProblem.h"

registerMooseAction("ApolloApp", AddTransferLagrangeVecAction, "add_transfer_lagrange_vec");

InputParameters
AddTransferLagrangeVecAction::validParams()
{
  InputParameters params = Action::validParams();
  params.addClassDescription(
      "Creates an auxiliary Lagrange vector and three hidden standard Lagrange variables for the "
      "components. This allows for MultiApp transfer.");

  MooseEnum orders("CONSTANT FIRST SECOND THIRD FOURTH", "FIRST", true);
  params.addParam<MooseEnum>("order", orders, "Specifies the order of the Lagrange vector.");

  return params;
}

AddTransferLagrangeVecAction::AddTransferLagrangeVecAction(const InputParameters & parameters)
  : Action(parameters)
{
}

void
AddTransferLagrangeVecAction::act()
{
  // 1. Get the Lagrange vector order from the input.
  auto the_order = getParam<MooseEnum>("order");

  // 2. Construct the parameters for the Lagrange vector.
  auto vector_params = _factory.getValidParams("VectorMooseVariable");
  vector_params.set<MooseEnum>("family") = "LAGRANGE_VEC";
  vector_params.set<MooseEnum>("order") = the_order;

  // 3. Construct the parameters for the xyz components of the Lagrange vector.
  // NB: They should be of the same order.
  auto component_params = _factory.getValidParams("MooseVariable");

  component_params.set<MooseEnum>("family") = "LAGRANGE";
  component_params.set<MooseEnum>("order") = the_order;

  _problem->addAuxVariable("VectorMooseVariable", name(), vector_params);

  _problem->addAuxVariable("MooseVariable", name() + "_x", component_params);
  _problem->addAuxVariable("MooseVariable", name() + "_y", component_params);
  _problem->addAuxVariable("MooseVariable", name() + "_z", component_params);
}
