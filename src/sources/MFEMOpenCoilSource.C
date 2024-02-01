#include "MFEMOpenCoilSource.h"

registerMooseObject("ApolloApp", MFEMOpenCoilSource);

InputParameters
MFEMOpenCoilSource::validParams()
{
  InputParameters params = MFEMSource::validParams();

  params.addParam<UserObjectName>(
      "source_electric_field_gridfunction",
      "The gridfunction to store the electric field of the current source.");

  params.addParam<UserObjectName>("source_potential_gridfunction",
                                  "The gridfunction to store the scalar potential in the coil.");

  params.addParam<UserObjectName>("source_current_density_gridfunction",
                                  "",
                                  "The gridfunction to store the source current density.");

  params.addParam<UserObjectName>(
      "total_current_coef",
      "The total current ($I$) flowing through the coil. May be time dependent.");

  params.addParam<BoundaryName>(
      "coil_in_boundary",
      "The boundary (id or name) from the mesh determinining the coil input terminal");

  params.addParam<BoundaryName>(
      "coil_out_boundary",
      "The boundary (id or name) from the mesh determinining the coil output terminal");

  params.addParam<Real>(
      "l_tol",
      1.0e-20,
      "The relative tolerance to use in the linear solver for the OpenCoilSource");

  params.addParam<Real>(
      "l_abs_tol",
      1.0e-20,
      "The absolute tolerance to use in the linear solver for the OpenCoilSource");

  params.addParam<unsigned int>(
      "l_max_its",
      1000,
      "The number of iterations to use in the linear solver for the OpenCoilSource");

  params.addParam<int>(
      "print_level", 1, "The print levelof the linear solver for the OpenCoilSource");

  return params;
}

MFEMOpenCoilSource::MFEMOpenCoilSource(const InputParameters & parameters)
  : MFEMSource(parameters),
    _source_electric_field_gridfunction(
        getUserObject<MFEMVariable>("source_electric_field_gridfunction")),
    _source_potential_gridfunction(getUserObject<MFEMVariable>("source_potential_gridfunction")),
    _total_current_coef(getUserObject<MFEMCoefficient>("total_current_coef")),
    _conductivity_coef_name(std::string("electrical_conductivity")),
    _solver_params({{"Tolerance", float(getParam<Real>("l_tol"))},
                    {"AbsTolerance", float(getParam<Real>("l_abs_tol"))},
                    {"MaxIter", getParam<unsigned int>("l_max_its")},
                    {"PrintLevel", getParam<int>("print_level")}}),
    _coil_domains(blocks.size()),
    _coil_in_id(std::stoi(getParam<BoundaryName>("coil_in_boundary"))),
    _coil_out_id(std::stoi(getParam<BoundaryName>("coil_out_boundary"))),
    _electrodes(std::make_pair(_coil_in_id, _coil_out_id))
{
  for (unsigned int bid = 0; bid < blocks.size(); ++bid)
  {
    _coil_domains[bid] = blocks[bid];
  }

  _source = std::make_shared<hephaestus::OpenCoilSolver>(
      _source_electric_field_gridfunction.name(),
      _source_potential_gridfunction.name(),
      _total_current_coef.name(),
      _conductivity_coef_name,
      _coil_domains,
      _electrodes,
      true,
      isParamValid("source_current_density_gridfunction")
          ? getUserObject<MFEMVariable>("source_current_density_gridfunction").name()
          : "",
      _solver_params);
}
