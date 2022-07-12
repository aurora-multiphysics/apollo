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

// TODO: Currently assumes the vector function coefficient is 3D
MFEMVectorFunctionDirichletBC::MFEMVectorFunctionDirichletBC(const InputParameters & parameters)
  : MFEMBoundaryCondition(parameters),
    _func(getFunction("function")),
    _vec_function_coef(3, [&](const mfem::Vector & p, double t, mfem::Vector & u) {
      libMesh::RealVectorValue vector_value = _func.vectorValue(t, PointFromMFEMVector(p));
      u[0] = vector_value(0);
      u[1] = vector_value(1);
      u[2] = vector_value(2);
    })
{
  _boundary_condition = new hephaestus::VectorFunctionDirichletBC(
      getParam<std::string>("variable"), bdr_attr, &_vec_function_coef);
}

hephaestus::BoundaryCondition *
MFEMVectorFunctionDirichletBC::getBC()
{
  return _boundary_condition;
}

void
MFEMVectorFunctionDirichletBC::storeCoefficients(hephaestus::DomainProperties & domain_properties)
{
  std::string vec_coef_name = std::string("bc_vec_coef_") + getParam<std::string>("_object_name");
  domain_properties.vector_property_map[vec_coef_name] = &_vec_function_coef;
}

MFEMVectorFunctionDirichletBC::~MFEMVectorFunctionDirichletBC() {}
