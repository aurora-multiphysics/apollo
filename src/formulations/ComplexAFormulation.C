#include "ComplexAFormulation.h"

registerMooseObject("ApolloApp", ComplexAFormulation);

InputParameters
ComplexAFormulation::validParams()
{
  InputParameters params = MFEMFormulation::validParams();
  params.addParam<std::string>("magnetic_vector_potential_re_name",
                               "Name of H(curl) conforming MFEM gridfunction representing real "
                               "component of complex electric field");
  params.addParam<std::string>("magnetic_vector_potential_im_name",
                               "Name of H(curl) conforming MFEM gridfunction representing "
                               "imaginary component of complex electric field");
  params.addParam<std::string>("magnetic_vector_potential_name",
                               "Name of complex H(curl) conforming MFEM gridfunction to represent "
                               "complex electric field");
  params.addParam<std::string>("frequency_name",
                               "Name of MFEM coefficient representing mode frequency (Hz)");
  params.addParam<std::string>("magnetic_permeability_name",
                               "Name of MFEM coefficient representing magnetic permeability");
  params.addParam<std::string>("electric_conductivity_name",
                               "Name of MFEM coefficient representing electric conductivity");
  params.addParam<std::string>("dielectric_permittivity_name",
                               "Name of MFEM coefficient representing dielectric permittivity");
  params.addParam<std::string>("magnetic_reluctivity_name",
                               "Name of MFEM coefficient to be created to represent magnetic "
                               "reluctivity (reciprocal of permeability)");
  return params;
}

ComplexAFormulation::ComplexAFormulation(const InputParameters & parameters)
  : MFEMFormulation(parameters),
    magnetic_vector_potential_name(getParam<std::string>("magnetic_vector_potential_name")),
    magnetic_vector_potential_re_name(getParam<std::string>("magnetic_vector_potential_re_name")),
    magnetic_vector_potential_im_name(getParam<std::string>("magnetic_vector_potential_im_name")),
    frequency_name(getParam<std::string>("frequency_name")),
    magnetic_permeability_name(getParam<std::string>("magnetic_permeability_name")),
    electric_conductivity_name(getParam<std::string>("electric_conductivity_name")),
    dielectric_permittivity_name(getParam<std::string>("dielectric_permittivity_name")),
    magnetic_reluctivity_name(getParam<std::string>("magnetic_reluctivity_name")),
    formulation(magnetic_reluctivity_name,
                electric_conductivity_name,
                dielectric_permittivity_name,
                frequency_name,
                magnetic_vector_potential_name)
{
}

ComplexAFormulation::~ComplexAFormulation() {}
