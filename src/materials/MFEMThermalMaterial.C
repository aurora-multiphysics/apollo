#include "MFEMThermalMaterial.h"

registerMooseObject("ApolloApp", MFEMThermalMaterial);

InputParameters
MFEMThermalMaterial::validParams()
{
  InputParameters params = MFEMMaterial::validParams();

  params.addParam<UserObjectName>("thermal_conductivity_coeff",
                                  "The thermal conductivity ($\\k$) of the conductor.");

  params.addParam<UserObjectName>("heat_capacity_coeff",
                                  "The heat capacity ($\\c$) of the conductor.");

  return params;
}

MFEMThermalMaterial::MFEMThermalMaterial(const InputParameters & parameters)
  : MFEMMaterial(parameters),
    // Get the parameters from the input file
    // Create mfem::Coefficients to represent materials
    _heat_capacity_coeff(&getUserObject<MFEMCoefficient>("thermal_conductivity_coeff")),
    _thermal_conductivity_coeff(&getUserObject<MFEMCoefficient>("heat_capacity_coeff"))
{
}

void
MFEMThermalMaterial::storeCoefficients(hephaestus::Subdomain & subdomain)
{
  subdomain._scalar_coefficients.Register("thermal_conductivity",
                                          _thermal_conductivity_coeff->getCoefficient());
  subdomain._scalar_coefficients.Register("heat_capacity", _heat_capacity_coeff->getCoefficient());
}

MFEMThermalMaterial::~MFEMThermalMaterial() {}
