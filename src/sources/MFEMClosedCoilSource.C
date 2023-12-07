#include "MFEMClosedCoilSource.h"

registerMooseObject("ApolloApp", MFEMClosedCoilSource);

InputParameters
MFEMClosedCoilSource::validParams()
{
  InputParameters params = MFEMSource::validParams();

  params.addRequiredParam<UserObjectName>(
      "total_current_coefficient",
      "The total current ($I$) flowing through the coil. May be time dependent.");

  params.addRequiredParam<UserObjectName>(
      "hcurl_fespace",
      "The FESpace associated with the test variable the source will be applied to.");

  params.addRequiredParam<UserObjectName>(
      "h1_fespace",
      "The FESpace associated with the scalar potential used in the ClosedCoilSolver.");

  params.addParam<UserObjectName>("source_current_density_dual_gridfunction",
                                  "The gridfunction to store the source current density.");

  params.addParam<BoundaryName>("coil_xsection_boundary",
                                "A boundary (id or name) specifying a cross section of the coil in "
                                "mesh, at which the current will be imposed");
  return params;
}

MFEMClosedCoilSource::MFEMClosedCoilSource(const InputParameters & parameters)
  : MFEMSource(parameters),
    _source_current_density_dual_gridfunction(
        getUserObject<MFEMVariable>("source_current_density_dual_gridfunction")),
    _hcurl_fespace(getUserObject<MFEMFESpace>("hcurl_fespace")),
    _h1_fespace(getUserObject<MFEMFESpace>("h1_fespace")),
    _total_current_coef(getUserObject<MFEMCoefficient>("total_current_coefficient")),
    _closed_coil_params({{"JGridFunctionName", _source_current_density_dual_gridfunction.name()},
                         {"HCurlFESpaceName", _hcurl_fespace.name()},
                         {"H1FESpaceName", _h1_fespace.name()},
                         {"IFuncCoefName", _total_current_coef.name()},
                         {"Jtransfer", true}}),
    _coil_domains(blocks.size()),
    _coil_xsection_id(std::stoi(getParam<BoundaryName>("coil_xsection_boundary")))
{
  for (unsigned int bid = 0; bid < blocks.size(); ++bid)
  {
    _coil_domains[bid] = blocks[bid];
  }
  _source = new hephaestus::ClosedCoilSolver(_closed_coil_params, _coil_domains, _coil_xsection_id);
}

hephaestus::Source *
MFEMClosedCoilSource::getSource()
{
  return _source;
}

void
MFEMClosedCoilSource::storeCoefficients(hephaestus::Coefficients & coefficients)
{
}

MFEMClosedCoilSource::~MFEMClosedCoilSource() {}
