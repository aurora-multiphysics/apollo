#include "MFEMClosedCoilSource.h"

registerMooseObject("ApolloApp", MFEMClosedCoilSource);

InputParameters
MFEMClosedCoilSource::validParams()
{
  InputParameters params = MFEMSource::validParams();

  params.addParam<UserObjectName>("source_electric_field",
                                  "The gridfunction to store the electric field of the source.");

  params.addRequiredParam<UserObjectName>(
      "hcurl_fespace",
      "The FESpace associated with the test variable the source will be applied to.");

  params.addRequiredParam<UserObjectName>(
      "h1_fespace",
      "The FESpace associated with the scalar potential used in the ClosedCoilSolver.");

  params.addRequiredParam<UserObjectName>(
      "total_current_coefficient",
      "The total current ($I$) flowing through the coil. May be time dependent.");

  params.addParam<BoundaryName>("coil_xsection_boundary",
                                "A boundary (id or name) specifying a cross section of the coil in "
                                "mesh, at which the current will be imposed");
  return params;
}

MFEMClosedCoilSource::MFEMClosedCoilSource(const InputParameters & parameters)
  : MFEMSource(parameters),
    _source_electric_field(getUserObject<MFEMVariable>("source_electric_field")),
    _hcurl_fespace(getUserObject<MFEMFESpace>("hcurl_fespace")),
    _h1_fespace(getUserObject<MFEMFESpace>("h1_fespace")),
    _total_current_coef(getUserObject<MFEMCoefficient>("total_current_coefficient")),
    _conductivity_coef_name(std::string("electrical_conductivity")),
    _coil_domains(blocks.size()),
    _coil_xsection_id(std::stoi(getParam<BoundaryName>("coil_xsection_boundary"))),
    _solver_params({{"Tolerance", float(getParam<Real>("l_tol"))},
                    {"AbsTolerance", float(getParam<Real>("l_abs_tol"))},
                    {"MaxIter", getParam<unsigned int>("l_max_its")},
                    {"PrintLevel", getParam<int>("print_level")}})
{
  for (unsigned int bid = 0; bid < blocks.size(); ++bid)
  {
    _coil_domains[bid] = blocks[bid];
  }

  _source = std::make_shared<hephaestus::ClosedCoilSolver>(
      _source_electric_field.name(),
      _hcurl_fespace.name(),
      _h1_fespace.name(),
      _total_current_coef.name(),
      _conductivity_coef_name,
      _coil_domains,
      _coil_xsection_id,
      true,
      isParamValid("source_current_density_gridfunction")
          ? getUserObject<MFEMVariable>("source_current_density_gridfunction").name()
          : "",
      _solver_params);
}
