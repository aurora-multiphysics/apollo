#include "MFEMConductor.h"
#include "Function.h"

registerMooseObject("ApolloApp", MFEMConductor);

const Real MFEMConductor::_mu0 = 1.25663706e-6;
const Real MFEMConductor::_epsilon0 = 8.85418782e-12;

InputParameters
MFEMConductor::validParams()
{
  InputParameters params = GeneralUserObject::validParams();

  params.set<std::string>("_moose_base") = "MaterialBase";
  params.addPrivateParam<bool>("_neighbor", false);
  params.addPrivateParam<bool>("_interface", false);

  // Parameter for radius of the spheres used to interpolate permeability.
  params.addParam<Real>(
      "electrical_conductivity",
      0.0,
      "The electrical_conductivity ($\\sigma$) of the conductor. Defaults to 0.0");
  params.addParam<Real>(
      "rel_permittivity",
      1.0,
      "The relative permittivity ($\\varepsilon$) of the conductor. Defaults to 1.0");
  params.addParam<Real>("rel_permeability",
                        1.0,
                        "The relative permeability ($\\mu$) of the conductor. Defaults to 1.0");
  params.addParam<Real>("thermal_conductivity",
                        0.0,
                        "The thermal conductivity ($\\k$) of the conductor. Defaults to 0.0");
  params.addParam<Real>(
      "heat_capacity", 0.0, "The heat capacity ($\\c$) of the conductor. Defaults to 0.0");
  params.addParam<std::vector<SubdomainName>>(
      "block", "The list of blocks (ids or names) that this object will be applied");
  return params;
}

MFEMConductor::MFEMConductor(const InputParameters & parameters)
  : GeneralUserObject(parameters),
    // Get the parameters from the input file
    _input_electrical_conductivity(getParam<Real>("electrical_conductivity")),
    _input_rel_permittivity(getParam<Real>("rel_permittivity")),
    _input_rel_permeability(getParam<Real>("rel_permeability")),
    _input_thermal_conductivity(getParam<Real>("thermal_conductivity")),
    _input_heat_capacity(getParam<Real>("heat_capacity")),

    // Create mfem::Coefficients to represent materials
    _electrical_conductivity_coeff(mfem::ConstantCoefficient(_input_electrical_conductivity)),
    _permittivity_coeff(mfem::ConstantCoefficient(_epsilon0 * _input_rel_permittivity)),
    _permeability_coeff(mfem::ConstantCoefficient(_mu0 * _input_rel_permeability)),
    _resistivity_coeff(mfem::ConstantCoefficient(1.0 / _input_electrical_conductivity)),
    _reluctivity_coeff(mfem::ConstantCoefficient(1.0 / (_mu0 * _input_rel_permeability))),

    _heat_capacity_coeff(mfem::ConstantCoefficient(_input_heat_capacity)),
    _inverse_heat_capacity_coeff(mfem::ConstantCoefficient(1.0 / _input_heat_capacity)),
    _thermal_conductivity_coeff(mfem::ConstantCoefficient(_input_thermal_conductivity)),
    _inverse_thermal_conductivity_coeff(
        mfem::ConstantCoefficient(1.0 / _input_thermal_conductivity))
{
  scalar_property_map["electrical_conductivity"] = &_electrical_conductivity_coeff;
  scalar_property_map["thermal_conductivity"] = &_thermal_conductivity_coeff;
  scalar_property_map["inverse_thermal_conductivity"] = &_inverse_thermal_conductivity_coeff;
  scalar_property_map["heat_capacity"] = &_heat_capacity_coeff;
  scalar_property_map["inverse_heat_capacity"] = &_inverse_heat_capacity_coeff;
}

MFEMConductor::~MFEMConductor() {}
