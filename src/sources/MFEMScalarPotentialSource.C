#include "MFEMScalarPotentialSource.h"

registerMooseObject("ApolloApp", MFEMScalarPotentialSource);

InputParameters
MFEMScalarPotentialSource::validParams()
{
  InputParameters params = MFEMSource::validParams();
  params.addRequiredParam<std::string>(
      "potential",
      "Name of the potential used in the solver, necessary to find boundary conditions");
  params.addRequiredParam<std::string>("grad_potential", "Name of the gradient of the potential.");
  params.addRequiredParam<std::string>(
      "conductivity", "Name of the conductivity coefficient associated with the potential.");
  params.addRequiredParam<UserObjectName>("hcurl_fespace",
                                          "The H(Curl) FE space to use in the source.");
  params.addRequiredParam<UserObjectName>("h1_fespace", "The H1 FE space to use in the source.");
  params.addParam<float>("solver_l_tol",
                         "Tolerance for the linear solver used when performing divergence "
                         "cleaning. Defaults to the same value used by the Executioner.");
  params.addParam<unsigned int>(
      "solver_max_its",
      "Maximum number of iterations allowed for the linear solver used when performing divergence "
      "cleaning. Defaults to the same value used by the Executioner.");
  return params;
}

MFEMScalarPotentialSource::MFEMScalarPotentialSource(const InputParameters & parameters)
  : MFEMSource(parameters),
    source_coef_name(std::string("source_") + getParam<std::string>("_object_name")),
    potential_name(getParam<std::string>("potential")),
    grad_potential_name(getParam<std::string>("grad_potential")),
    conductivity_coef_name(getParam<std::string>("conductivity")),
    hcurl_fespace(getUserObject<MFEMFESpace>("hcurl_fespace")),
    h1_fespace(getUserObject<MFEMFESpace>("h1_fespace"))
{
  hephaestus::InputParameters _solver_options;
  EquationSystems & es = getParam<FEProblemBase *>("_fe_problem_base")->es();
  _solver_options.SetParam("Tolerance",
                           parameters.isParamSetByUser("solver_l_tol")
                               ? getParam<float>("solver_l_tol")
                               : float(es.parameters.get<Real>("linear solver tolerance")));
  _solver_options.SetParam(
      "MaxIter",
      parameters.isParamSetByUser("solver_max_its")
          ? getParam<unsigned int>("solver_max_its")
          : es.parameters.get<unsigned int>("linear solver maximum iterations"));
  _solver_options.SetParam("PrintLevel", -1);

  hephaestus::InputParameters scalar_potential_source_params;
  scalar_potential_source_params.SetParam("SourceName", source_coef_name);
  scalar_potential_source_params.SetParam("PotentialName", potential_name);
  scalar_potential_source_params.SetParam("GradPotentialName", grad_potential_name);
  scalar_potential_source_params.SetParam("ConductivityCoefName", conductivity_coef_name);
  scalar_potential_source_params.SetParam("HCurlFESpaceName", hcurl_fespace.name());
  scalar_potential_source_params.SetParam("H1FESpaceName", h1_fespace.name());
  scalar_potential_source_params.SetParam("SolverOptions", _solver_options);

  _source = new hephaestus::ScalarPotentialSource(scalar_potential_source_params);
}

hephaestus::Source *
MFEMScalarPotentialSource::getSource()
{
  return _source;
}

void
MFEMScalarPotentialSource::storeCoefficients(hephaestus::Coefficients & coefficients)
{
  // coefficients.vectors[source_coef_name] = _restricted_coef;
}

MFEMScalarPotentialSource::~MFEMScalarPotentialSource() {}
