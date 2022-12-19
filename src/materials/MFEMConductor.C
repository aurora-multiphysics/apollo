#include "MFEMConductor.h"
#include "Function.h"

registerMooseObject("ApolloApp", MFEMConductor);

const Real MFEMConductor::_mu0 = 1.25663706e-6;
const Real MFEMConductor::_epsilon0 = 8.85418782e-12;

InputParameters
MFEMConductor::validParams()
{
  InputParameters params = MFEMMaterial::validParams();

  params.addParam<UserObjectName>(
      "electrical_conductivity_coeff",
      "The electrical conductivity coefficient ($\\sigma$) of the conductor. Defaults to 0.0");

  params.addParam<UserObjectName>(
      "electric_permittivity_coeff",
      "The electric permittivity coefficient ($\\varepsilon$) of the conductor. Defaults to 1.0");

  params.addParam<UserObjectName>(
      "magnetic_permeability_coeff",
      "The magnetic permeability ($\\mu$) of the conductor. Defaults to 1.0");

  params.addParam<UserObjectName>(
      "thermal_conductivity_coeff",
      "The thermal conductivity ($\\k$) of the conductor. Defaults to 0.0");

  params.addParam<UserObjectName>("heat_capacity_coeff",
                                  "The heat capacity ($\\c$) of the conductor. Defaults to 0.0");

  return params;
}

MFEMConductor::MFEMConductor(const InputParameters & parameters)
  : MFEMMaterial(parameters),
    // Get the parameters from the input file
    // Create mfem::Coefficients to represent materials
    _electrical_conductivity_coeff(
        getUserObject<mfem::Coefficient>("electrical_conductivity_coeff")),
    _permittivity_coeff(getUserObject<mfem::Coefficient>("electric_permittivity_coeff")),
    _permeability_coeff(getUserObject<mfem::Coefficient>("magnetic_permeability_coeff")),
    _heat_capacity_coeff(getUserObject<mfem::Coefficient>("thermal_conductivity_coeff")),
    _thermal_conductivity_coeff(getUserObject<mfem::Coefficient>("heat_capacity_coeff"))
{
}

void
MFEMConductor::storeCoefficients(hephaestus::Subdomain & subdomain)
{
  subdomain.property_map["electrical_conductivity"] =
      const_cast<mfem::Coefficient *>(&_electrical_conductivity_coeff);
  subdomain.property_map["magnetic_permeability"] =
      const_cast<mfem::Coefficient *>(&_permeability_coeff);
  subdomain.property_map["thermal_conductivity"] =
      const_cast<mfem::Coefficient *>(&_thermal_conductivity_coeff);
  subdomain.property_map["heat_capacity"] = const_cast<mfem::Coefficient *>(&_heat_capacity_coeff);
}

MFEMConductor::~MFEMConductor() {}
