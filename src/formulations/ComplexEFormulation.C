#include "ComplexEFormulation.h"

registerMooseObject("ApolloApp", ComplexEFormulation);

InputParameters
ComplexEFormulation::validParams()
{
  InputParameters params = MFEMFormulation::validParams();
  params.addParam<std::string>("e_field_re_name",
                               "Name of H(curl) conforming MFEM gridfunction representing real "
                               "component of complex electric field");
  params.addParam<std::string>("e_field_im_name",
                               "Name of H(curl) conforming MFEM gridfunction representing "
                               "imaginary component of complex electric field");
  params.addParam<std::string>("magnetic_permeability_name",
                               "Name of MFEM coefficient representing magnetic permeability");
  params.addParam<std::string>("electric_conductivity_name",
                               "Name of MFEM coefficient representing electric conductivity");
  params.addParam<std::string>("dielectric_permittivity_name",
                               "Name of MFEM coefficient representing dielectric permittivity");
  return params;
}

ComplexEFormulation::ComplexEFormulation(const InputParameters & parameters)
  : MFEMFormulation(parameters),
    e_field_re_name(getParam<std::string>("e_field_re_name")),
    e_field_im_name(getParam<std::string>("e_field_im_name")),
    magnetic_permeability_name(getParam<std::string>("magnetic_permeability_name")),
    electric_conductivity_name(getParam<std::string>("electric_conductivity_name")),
    dielectric_permittivity_name(getParam<std::string>("dielectric_permittivity_name")),
    formulation()
{
}

ComplexEFormulation::~ComplexEFormulation() {}
