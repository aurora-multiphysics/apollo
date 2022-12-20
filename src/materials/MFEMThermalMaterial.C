#include "MFEMThermalMaterial.h"
#include "Function.h"

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
    _heat_capacity_coeff(getUserObject<mfem::Coefficient>("thermal_conductivity_coeff")),
    _thermal_conductivity_coeff(getUserObject<mfem::Coefficient>("heat_capacity_coeff"))
{
}

void
MFEMThermalMaterial::storeCoefficients(hephaestus::Subdomain & subdomain)
{
  subdomain.property_map["thermal_conductivity"] =
      const_cast<mfem::Coefficient *>(&_thermal_conductivity_coeff);
  subdomain.property_map["heat_capacity"] = const_cast<mfem::Coefficient *>(&_heat_capacity_coeff);
}

MFEMThermalMaterial::~MFEMThermalMaterial() {}
