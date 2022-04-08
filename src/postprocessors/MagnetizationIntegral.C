//* Replace with ElementIntegralArrayVariablePostprocessor?

#include "MagnetizationIntegral.h"

registerMooseObject("ApolloApp", MagnetizationIntegral);

InputParameters
MagnetizationIntegral::validParams()
{
  InputParameters params = ElementIntegralPostprocessor::validParams();
  MooseEnum component("x y z");
  params.addRequiredCoupledVar("magnetic_field", "The magnetic field (H) in the system.");
  params.addRequiredParam<MooseEnum>(
      "component", component, "The desired component of magnetization.");
  params.addClassDescription("Computes the magnetisation (m) of the block.");
  return params;
}

MagnetizationIntegral::MagnetizationIntegral(const InputParameters & parameters)
  : ElementIntegralPostprocessor(parameters),
    _component(getParam<MooseEnum>("component")),
    _u(coupledVectorValue("magnetic_field")),
    _curl_u(coupledCurl("magnetic_field"))
{
}

Real
MagnetizationIntegral::computeQpIntegral()
{
  return 0.5 * _q_point[_qp].cross(_curl_u[_qp])(_component);
}
