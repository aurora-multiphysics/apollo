#include "VectorVariableFromComponentsAux.h"
#include "ApolloVectorTransferFlags.h"

registerMooseObject("MooseApp", VectorVariableFromComponentsAux);

InputParameters
VectorVariableFromComponentsAux::validParams()
{
  InputParameters params = WritableVectorAuxKernel::validParams();

  ExecFlagEnum & exec = params.set<ExecFlagEnum>("execute_on");
  exec.addAvailableFlags(ApolloApp::EXEC_RECOVER_VECTOR_POST_TRANSFER);

  params.addRequiredCoupledVar("component_x", "The x-component of the vector variable.");
  params.addRequiredCoupledVar("component_y", "The y-component of the vector variable.");
  params.addRequiredCoupledVar("component_z", "The z-component of the vector variable.");

  params.addClassDescription("Combine three standard variables into a vector variable.");

  return params;
}

VectorVariableFromComponentsAux::VectorVariableFromComponentsAux(const InputParameters & parameters)
  : WritableVectorAuxKernel(parameters),
    _component_x(getNonWritableCoupledVariable("component_x")),
    _component_y(getNonWritableCoupledVariable("component_y")),
    _component_z(getNonWritableCoupledVariable("component_z"))
{
  checkVectorComponents(_component_x, _component_y, _component_z);
}

void
VectorVariableFromComponentsAux::compute()
{
  Real value_x = _component_x.dofValues()[0];
  Real value_y = _component_y.dofValues()[0];
  Real value_z = _component_z.dofValues()[0];

  _variable->setDofValue(value_x, 0);
  _variable->setDofValue(value_y, 1);
  _variable->setDofValue(value_z, 2);
}
