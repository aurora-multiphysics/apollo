#include "ElementVectorL2Difference.h"

registerMooseObject("MooseApp", ElementVectorL2Difference);

InputParameters
ElementVectorL2Difference::validParams()
{
  InputParameters params = ElementIntegralPostprocessor::validParams();

  params.addRequiredCoupledVar("var", "The name of the vector variable");
  params.addRequiredCoupledVar("other_var", "The name of the other vector variable to compare against");

  params.addClassDescription("Computes the element-wise L2 difference between two coupled vector fields.");
  return params;
}

ElementVectorL2Difference::ElementVectorL2Difference(const InputParameters & parameters)
  : ElementIntegralPostprocessor(parameters),
    _uvw(coupledVectorValue("var")),
    _other_uvw(coupledVectorValue("other_var"))
{
}

Real
ElementVectorL2Difference::getValue() const
{
  return std::sqrt(ElementIntegralPostprocessor::getValue());
}

Real
ElementVectorL2Difference::computeQpIntegral()
{
  RealVectorValue solution_value(0.0, 0.0, 0.0);
  RealVectorValue other_value(0.0, 0.0, 0.0);

  for (int i = 0; i < 3; i++)
  {
    solution_value(i) = _uvw[_qp](i);
    other_value(i) = _other_uvw[_qp](i);
  }

  RealVectorValue difference_vector = (solution_value - other_value);

  return difference_vector.norm_sq();  // dot product of difference vector.
}

