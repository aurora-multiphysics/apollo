//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ElementNedelecL2Error.h"
#include "Function.h"

registerMooseObject("ApolloApp", ElementNedelecL2Error);

defineLegacyParams(ElementNedelecL2Error);

InputParameters
ElementNedelecL2Error::validParams()
{
  InputParameters params = ElementIntegralPostprocessor::validParams();
  params.addParam<FunctionName>("function", 0, "The analytic solution to compare against");
  params.addCoupledVar("variable", 0, "The FE solution vector.");

  params.addClassDescription("Computes the Vector L2 difference of up to three variables "
                             "simultaneously (normally x, y, z)");
  return params;
}

ElementNedelecL2Error::ElementNedelecL2Error(const InputParameters & parameters)
  : ElementIntegralPostprocessor(parameters),
    _function(getFunction("function")),
    _u(coupledVectorValue("variable"))
{
}

Real
ElementNedelecL2Error::getValue()
{
  return std::sqrt(ElementIntegralPostprocessor::getValue());
}

Real
ElementNedelecL2Error::computeQpIntegral()
{
  RealVectorValue u_exact;
  u_exact = _function.vectorValue(_t, _q_point[_qp]);
  return (_u[_qp] - u_exact).norm_sq(); // dot product of difference vector
}
