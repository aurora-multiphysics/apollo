#include "MFEMVectorNormalIntegratedBC.h"
#include "Function.h"

registerMooseObject("ApolloApp", MFEMVectorNormalIntegratedBC);

InputParameters
MFEMVectorNormalIntegratedBC::validParams()
{
  InputParameters params = MFEMBoundaryCondition::validParams();
  params.addParam<FunctionName>(
      "function", 0, "The vector function to whose normal component is used in the integrated BC");
  return params;
}

// TODO: Currently assumes the vector function coefficient is 3D
MFEMVectorNormalIntegratedBC::MFEMVectorNormalIntegratedBC(const InputParameters & parameters)
  : MFEMBoundaryCondition(parameters),
    _func(getFunction("function")),
    _vec_function_coef(3, [&](const mfem::Vector & p, double t, mfem::Vector & u) {
      libMesh::RealVectorValue vector_value = _func.vectorValue(t, PointFromMFEMVector(p));
      u[0] = vector_value(0);
      u[1] = vector_value(1);
      u[2] = vector_value(2);
    })
{
  _boundary_condition =
      new hephaestus::IntegratedBC(getParam<std::string>("variable"),
                                   bdr_attr,
                                   new mfem::BoundaryNormalLFIntegrator(_vec_function_coef));
}

hephaestus::BoundaryCondition *
MFEMVectorNormalIntegratedBC::getBC()
{
  return _boundary_condition;
}

void
MFEMVectorNormalIntegratedBC::storeCoefficients(hephaestus::Coefficients & coefficients)
{
  std::string vec_coef_name = std::string("bc_vec_coef_") + getParam<std::string>("_object_name");
  coefficients.vectors.Register(vec_coef_name, &_vec_function_coef, true);
}

MFEMVectorNormalIntegratedBC::~MFEMVectorNormalIntegratedBC() {}
