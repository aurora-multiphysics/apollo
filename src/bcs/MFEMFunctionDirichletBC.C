#include "MFEMFunctionDirichletBC.h"
#include "Function.h"

registerMooseObject("ApolloApp", MFEMFunctionDirichletBC);

libMesh::Point
PointFromMFEMVector(const mfem::Vector & vec)
{
  return libMesh::Point(vec.Elem(0), vec.Elem(1), vec.Elem(2));
}

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
    _function_coeff(
        [&](const mfem::Vector & p, double t) { return _func.value(t, PointFromMFEMVector(p)); })
{
    _boundary_condition = new hephaestus::FunctionDirichletBC(getParam<std::string>("_object_name"), bdr_attr, &_function_coeff);
}

hephaestus::BoundaryCondition *
MFEMFunctionDirichletBC::getBC()
{
return _boundary_condition;
}

MFEMFunctionDirichletBC::~MFEMFunctionDirichletBC() {}
