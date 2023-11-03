#include "MFEMFunctionCoefficient.h"

registerMooseObject("ApolloApp", MFEMFunctionCoefficient);

InputParameters
MFEMFunctionCoefficient::validParams()
{
  InputParameters params = MFEMCoefficient::validParams();
  params.addParam<FunctionName>("function", 0, "The function to associated with the Dirichlet BC");
  return params;
}

MFEMFunctionCoefficient::MFEMFunctionCoefficient(const InputParameters & parameters)
  : MFEMCoefficient(parameters),
    _func(getFunction("function")),
    _coefficient([&](const mfem::Vector & p, double t)
                 { return _func.value(t, PointFromMFEMVector(p)); })
{
}

MFEMFunctionCoefficient::~MFEMFunctionCoefficient() {}
