#include "MFEMBoundaryCondition.h"
#include "Function.h"

registerMooseObject("ApolloApp", MFEMBoundaryCondition);

libMesh::Point
PointFromMFEMVector(const mfem::Vector & vec)
{
  return libMesh::Point(vec.Elem(0), vec.Elem(1), vec.Elem(2));
}

InputParameters
MFEMBoundaryCondition::validParams()
{
  InputParameters params = GeneralUserObject::validParams();

  params.registerBase("BoundaryCondition");

  // Create user-facing 'boundary' input for restricting inheriting object to boundaries
  params.addParam<std::vector<BoundaryName>>(
      "boundary",
      "The list of boundaries (ids or names) from the mesh where this boundary condition applies");
  params.addParam<std::string>("variable", "Variable on which to apply the boundary condition");
  return params;
}

MFEMBoundaryCondition::MFEMBoundaryCondition(const InputParameters & parameters)
  : GeneralUserObject(parameters),
    _boundary_names(getParam<std::vector<BoundaryName>>("boundary")),
    bdr_attr(_boundary_names.size())
{
  for (unsigned int i = 0; i < _boundary_names.size(); ++i)
  {
    bdr_attr[i] = std::stoi(_boundary_names[i]);
  }
  _boundary_condition =
      new hephaestus::BoundaryCondition(getParam<std::string>("variable"), bdr_attr);
}

hephaestus::BoundaryCondition *
MFEMBoundaryCondition::getBC()
{
  return _boundary_condition;
}

MFEMBoundaryCondition::~MFEMBoundaryCondition() { delete _boundary_condition; }
