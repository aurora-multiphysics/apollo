#include "MFEMFunctionDirichletBC.h"

registerMooseObject("ApolloApp", MFEMFunctionDirichletBC);

InputParameters
MFEMFunctionDirichletBC::validParams()
{
  InputParameters params = MFEMBoundaryCondition::validParams();
  params.addParam<FunctionName>("function", 0, "The function to associated with the Dirichlet BC");

  return params;
}

MFEMFunctionDirichletBC::MFEMFunctionDirichletBC(const InputParameters & parameters)
  : MFEMBoundaryCondition(parameters),
    _func(getFunction("function")),
    _function_coeff([&](const mfem::Vector & p, double t)
                    { return _func.value(t, PointFromMFEMVector(p)); })
{
  _boundary_condition = new hephaestus::FunctionDirichletBC(
      getParam<std::string>("variable"), bdr_attr, &_function_coeff);
}

hephaestus::BoundaryCondition *
MFEMFunctionDirichletBC::getBC()
{
  return _boundary_condition;
}

void
MFEMFunctionDirichletBC::storeCoefficients(hephaestus::Coefficients & coefficients)
{
  std::string coef_name = std::string("bc_coef_") + getParam<std::string>("_object_name");
  coefficients.scalars.Register(coef_name, &_function_coeff, true);
}
