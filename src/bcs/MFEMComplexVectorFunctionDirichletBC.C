#include "MFEMComplexVectorFunctionDirichletBC.h"
#include "Function.h"

registerMooseObject("ApolloApp", MFEMComplexVectorFunctionDirichletBC);

InputParameters
MFEMComplexVectorFunctionDirichletBC::validParams()
{
  InputParameters params = MFEMBoundaryCondition::validParams();
  params.addParam<FunctionName>(
      "real_function", 0, "The function to associated with the real component of the Dirichlet BC");
  params.addParam<FunctionName>(
      "imag_function",
      0,
      "The function to associated with the imaginary component of the Dirichlet BC");
  return params;
}

// TODO: Currently assumes the vector function coefficient is 3D
MFEMComplexVectorFunctionDirichletBC::MFEMComplexVectorFunctionDirichletBC(
    const InputParameters & parameters)
  : MFEMBoundaryCondition(parameters),
    _func_re(getFunction("real_function")),
    _func_im(getFunction("imag_function")),
    _vec_function_coef_re(3,
                          [&](const mfem::Vector & p, double t, mfem::Vector & u) {
                            libMesh::RealVectorValue vector_value =
                                _func_re.vectorValue(t, PointFromMFEMVector(p));
                            u[0] = vector_value(0);
                            u[1] = vector_value(1);
                            u[2] = vector_value(2);
                          }),
    _vec_function_coef_im(3, [&](const mfem::Vector & p, double t, mfem::Vector & u) {
      libMesh::RealVectorValue vector_value = _func_im.vectorValue(t, PointFromMFEMVector(p));
      u[0] = vector_value(0);
      u[1] = vector_value(1);
      u[2] = vector_value(2);
    })
{
  _boundary_condition = new hephaestus::VectorFunctionDirichletBC(
      getParam<std::string>("variable"), bdr_attr, &_vec_function_coef_re, &_vec_function_coef_im);
}

hephaestus::BoundaryCondition *
MFEMComplexVectorFunctionDirichletBC::getBC()
{
  return _boundary_condition;
}

void
MFEMComplexVectorFunctionDirichletBC::storeCoefficients(
    hephaestus::DomainProperties & domain_properties)
{
  std::string vec_coef_name_re =
      std::string("bc_vec_coef_re_") + getParam<std::string>("_object_name");
  domain_properties.vector_property_map[vec_coef_name_re] = &_vec_function_coef_re;
  std::string vec_coef_name_im =
      std::string("bc_vec_coef_im_") + getParam<std::string>("_object_name");
  domain_properties.vector_property_map[vec_coef_name_im] = &_vec_function_coef_im;
}

MFEMComplexVectorFunctionDirichletBC::~MFEMComplexVectorFunctionDirichletBC() {}
