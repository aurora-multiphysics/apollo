#include "MFEMBoundaryCondition.h"
#include "Function.h"

registerMooseObject("ApolloApp", MFEMBoundaryCondition);

InputParameters
MFEMBoundaryCondition::validParams()
{
  InputParameters params = GeneralUserObject::validParams();

  params.registerBase("BoundaryCondition");

  // Create user-facing 'boundary' input for restricting inheriting object to boundaries
  params.addParam<std::vector<BoundaryName>>(
      "boundary",
      "The list of boundaries (ids or names) from the mesh where this boundary condition applies");

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
      new hephaestus::BoundaryCondition(getParam<std::string>("_object_name"), bdr_attr);
}

hephaestus::BoundaryCondition *
MFEMBoundaryCondition::getBC()
{
  return _boundary_condition;
}

MFEMBoundaryCondition::~MFEMBoundaryCondition() {}
