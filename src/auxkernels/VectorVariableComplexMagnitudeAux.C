//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "VectorVariableComplexMagnitudeAux.h"

registerMooseObject("ApolloApp", VectorVariableComplexMagnitudeAux);

InputParameters
VectorVariableComplexMagnitudeAux::validParams()
{
  InputParameters params = AuxKernel::validParams();
  params.addClassDescription("Creates a field consisting of the magnitude of a "
                             "coupled vector variable.");
  params.addRequiredCoupledVar("real_vector",
                               "The variable from which to compute the component");
  params.addRequiredCoupledVar("imag_vector",
                               "The variable from which to compute the component");
  return params;
}

VectorVariableComplexMagnitudeAux::VectorVariableComplexMagnitudeAux(const InputParameters & parameters)
  : AuxKernel(parameters), _real_vector(coupledVectorValue("real_vector")),
  _imag_vector(coupledVectorValue("imag_vector"))
{
}

Real
VectorVariableComplexMagnitudeAux::computeValue()
{
  return std::sqrt(std::pow(_real_vector[_qp](0), 2) + std::pow(_real_vector[_qp](1), 2) +
                   std::pow(_real_vector[_qp](2), 2) + std::pow(_imag_vector[_qp](0), 2) + std::pow(_imag_vector[_qp](1), 2) +
                   std::pow(_imag_vector[_qp](2), 2));
}
