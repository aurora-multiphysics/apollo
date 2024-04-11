#include "MFEMScalarPotentialSource.h"

registerMooseObject("ApolloApp", MFEMScalarPotentialSource);

InputParameters
MFEMScalarPotentialSource::validParams()
{
  InputParameters params = MFEMSource::validParams();

  params.addParam<UserObjectName>("source_potential_gridfunction",
                                  "The gridfunction to store the scalar potential in the coil.");
  params.addParam<UserObjectName>(
      "source_grad_potential_gridfunction",
      "The gridfunction to store the potential gradient of the current source.");
  params.addRequiredParam<std::string>(
      "conductivity_coef", "Name of the conductivity coefficient associated with the potential.");
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
    _source_grad_potential_gridfunction(
        getUserObject<MFEMVariable>("source_grad_potential_gridfunction")),
    _source_potential_gridfunction(getUserObject<MFEMVariable>("source_potential_gridfunction")),
    _conductivity_coef_name(getParam<std::string>("conductivity_coef")),
    _hcurl_fespace(getUserObject<MFEMFESpace>("hcurl_fespace")),
    _h1_fespace(getUserObject<MFEMFESpace>("h1_fespace"))
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

  _source = std::make_shared<hephaestus::ScalarPotentialSource>(
      _source_grad_potential_gridfunction.name(),
      _source_potential_gridfunction.name(),
      _hcurl_fespace.name(),
      _h1_fespace.name(),
      _conductivity_coef_name,
      -1,
      _solver_options);
}
