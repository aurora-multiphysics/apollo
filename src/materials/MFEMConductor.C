#include "MFEMConductor.h"

registerMooseObject("ApolloApp", MFEMConductor);

const Real MFEMConductor::_mu0 = 1.25663706e-6;
const Real MFEMConductor::_epsilon0 = 8.85418782e-12;

InputParameters
MFEMConductor::validParams()
{
  InputParameters params = MFEMMaterial::validParams();

  params.addParam<UserObjectName>(
      "electrical_conductivity_coeff",
      "The electrical conductivity coefficient ($\\sigma$) of the conductor.");

  params.addParam<UserObjectName>(
      "electric_permittivity_coeff",
      "The electric permittivity coefficient ($\\varepsilon$) of the conductor.");

  params.addParam<UserObjectName>("magnetic_permeability_coeff",
                                  "The magnetic permeability ($\\mu$) of the conductor.");

  return params;
}

MFEMConductor::MFEMConductor(const InputParameters & parameters)
  : MFEMMaterial(parameters),
    // Get the parameters from the input file
    // Create mfem::Coefficients to represent materials
    _electrical_conductivity_coeff(
        &getUserObject<MFEMCoefficient>("electrical_conductivity_coeff")),
    _permittivity_coeff(&getUserObject<MFEMCoefficient>("electric_permittivity_coeff")),
    _permeability_coeff(&getUserObject<MFEMCoefficient>("magnetic_permeability_coeff"))
{
}

void
MFEMConductor::storeCoefficients(hephaestus::Subdomain & subdomain)
{
  subdomain._scalar_coefficients.Register("electrical_conductivity",
                                          _electrical_conductivity_coeff->getCoefficient());
  subdomain._scalar_coefficients.Register("magnetic_permeability",
                                          _permeability_coeff->getCoefficient());
  subdomain._scalar_coefficients.Register("dielectric_permittivity",
                                          _permittivity_coeff->getCoefficient());
}

MFEMConductor::~MFEMConductor() {}
