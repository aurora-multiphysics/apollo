#include "MFEMFunctionDirichletBC.h"

registerMooseObject("ApolloApp", MFEMFunctionDirichletBC);

InputParameters
MFEMFunctionDirichletBC::validParams()
{
  InputParameters params = MFEMBoundaryCondition::validParams();
  params.addRequiredParam<UserObjectName>(
      "function_coef", "The MFEM function coefficient to use in the Dirichlet condition");
  return params;
}

MFEMFunctionDirichletBC::MFEMFunctionDirichletBC(const InputParameters & parameters)
  : MFEMBoundaryCondition(parameters),
    _function_coef(const_cast<MFEMFunctionCoefficient *>(
        &getUserObject<MFEMFunctionCoefficient>("function_coef"))),
    _boundary_condition(
        getParam<std::string>("variable"), bdr_attr, _function_coef->getCoefficient())
{
}

hephaestus::BoundaryCondition *
MFEMFunctionDirichletBC::getBC()
{
  return &_boundary_condition;
}

void
MFEMFunctionDirichletBC::storeCoefficients(hephaestus::Coefficients & coefficients)
{
}
