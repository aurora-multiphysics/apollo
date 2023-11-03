#include "MFEMVectorNormalIntegratedBC.h"

registerMooseObject("ApolloApp", MFEMVectorNormalIntegratedBC);

InputParameters
MFEMVectorNormalIntegratedBC::validParams()
{
  InputParameters params = MFEMBoundaryCondition::validParams();
  params.addRequiredParam<UserObjectName>(
      "vector_coefficient",
      "The vector MFEM coefficient whose normal component will be used in the integrated BC");
  return params;
}

// TODO: Currently assumes the vector function coefficient is 3D
MFEMVectorNormalIntegratedBC::MFEMVectorNormalIntegratedBC(const InputParameters & parameters)
  : MFEMBoundaryCondition(parameters),
    _vec_coef(const_cast<MFEMVectorCoefficient *>(
        &getUserObject<MFEMVectorCoefficient>("vector_coefficient"))),
    _boundary_condition(getParam<std::string>("variable"),
                        bdr_attr,
                        new mfem::BoundaryNormalLFIntegrator(*_vec_coef->getVectorCoefficient()))
{
}

hephaestus::BoundaryCondition *
MFEMVectorNormalIntegratedBC::getBC()
{
  return &_boundary_condition;
}

MFEMVectorNormalIntegratedBC::~MFEMVectorNormalIntegratedBC() {}
