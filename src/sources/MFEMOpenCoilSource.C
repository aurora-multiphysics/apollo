#include "MFEMOpenCoilSource.h"

registerMooseObject("ApolloApp", MFEMOpenCoilSource);

InputParameters
MFEMOpenCoilSource::validParams()
{
  InputParameters params = MFEMSource::validParams();

  params.addParam<UserObjectName>(
      "total_current_coef",
      "The total current ($I$) flowing through the coil. May be time dependent.");

  params.addParam<UserObjectName>("source_current_density_gridfunction",
                                  "The gridfunction to store the source current density.");

  params.addParam<UserObjectName>("source_potential_gridfunction",
                                  "The gridfunction to store the scalar potential in the coil.");

  params.addParam<BoundaryName>(
      "coil_in_boundary",
      "The boundary (id or name) from the mesh determinining the coil input terminal");

  params.addParam<BoundaryName>(
      "coil_out_boundary",
      "The boundary (id or name) from the mesh determinining the coil output terminal");
  return params;
}

MFEMOpenCoilSource::MFEMOpenCoilSource(const InputParameters & parameters)
  : MFEMSource(parameters),
    _source_current_density_gridfunction(
        getUserObject<MFEMVariable>("source_current_density_gridfunction")),
    _source_potential_gridfunction(getUserObject<MFEMVariable>("source_potential_gridfunction")),
    _total_current_coef(getUserObject<MFEMCoefficient>("total_current_coef")),
    _conductivity_coef_name(std::string("electrical_conductivity")),
    _open_coil_params({{"SourceName", _source_current_density_gridfunction.name()},
                       {"PotentialName", _source_potential_gridfunction.name()},
                       {"IFuncCoefName", _total_current_coef.name()},
                       {"ConductivityCoefName", _conductivity_coef_name}}),
    _coil_domains(blocks.size()),
    _coil_in_id(std::stoi(getParam<BoundaryName>("coil_in_boundary"))),
    _coil_out_id(std::stoi(getParam<BoundaryName>("coil_out_boundary"))),
    _electrodes(std::make_pair(_coil_in_id, _coil_out_id))
{
  for (unsigned int bid = 0; bid < blocks.size(); ++bid)
  {
    _coil_domains[bid] = blocks[bid];
  }
  _source = new hephaestus::OpenCoilSolver(_open_coil_params, _coil_domains, _electrodes);
}

hephaestus::Source *
MFEMOpenCoilSource::getSource()
{
  return _source;
}

void
MFEMOpenCoilSource::storeCoefficients(hephaestus::Coefficients & coefficients)
{
}

MFEMOpenCoilSource::~MFEMOpenCoilSource() {}
