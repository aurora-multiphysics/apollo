#include "MFEMScalarDirichletBC.h"

registerMooseObject("ApolloApp", MFEMScalarDirichletBC);

InputParameters
MFEMScalarDirichletBC::validParams()
{
  InputParameters params = MFEMBoundaryCondition::validParams();
  params.addRequiredParam<UserObjectName>(
      "coefficient", "The scalar MFEM coefficient to use in the Dirichlet condition");
  return params;
}

MFEMScalarDirichletBC::MFEMScalarDirichletBC(const InputParameters & parameters)
  : MFEMBoundaryCondition(parameters),
    _coef(const_cast<MFEMCoefficient *>(&getUserObject<MFEMCoefficient>("coefficient"))),
    _boundary_condition(getParam<std::string>("variable"), bdr_attr, _coef->getCoefficient())
{
}

hephaestus::BoundaryCondition *
MFEMScalarDirichletBC::getBC()
{
  return &_boundary_condition;
}

void
MFEMScalarDirichletBC::storeCoefficients(hephaestus::Coefficients & coefficients)
{
}
