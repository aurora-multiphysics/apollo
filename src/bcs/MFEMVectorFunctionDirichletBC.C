#include "MFEMVectorFunctionDirichletBC.h"
#include "Function.h"

registerMooseObject("ApolloApp", MFEMVectorFunctionDirichletBC);

InputParameters
MFEMVectorFunctionDirichletBC::validParams()
{
  InputParameters params = MFEMBoundaryCondition::validParams();
  params.addParam<FunctionName>("function", 0, "The function to associated with the Dirichlet BC");
  return params;
}

MFEMVectorFunctionDirichletBC::MFEMVectorFunctionDirichletBC(const InputParameters & parameters)
  : MFEMBoundaryCondition(parameters),
    _func(getFunction("function")),
    _function_coeff(
        [&](const mfem::Vector & p, double t) { return _func.value(t, PointFromMFEMVector(p)); })
{
  _boundary_condition = new hephaestus::FunctionDirichletBC(
      getParam<std::string>("variable"), bdr_attr, &_function_coeff);
}

hephaestus::BoundaryCondition *
MFEMVectorFunctionDirichletBC::getBC()
{
  return _boundary_condition;
}

MFEMVectorFunctionDirichletBC::~MFEMVectorFunctionDirichletBC() {}
