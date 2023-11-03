#include "MFEMDivFreeVolumetricSource.h"

registerMooseObject("ApolloApp", MFEMDivFreeVolumetricSource);

InputParameters
MFEMDivFreeVolumetricSource::validParams()
{
  InputParameters params = MFEMSource::validParams();
  params.addRequiredParam<UserObjectName>(
      "vector_coefficient", "The vector MFEM coefficient providing the source to be divergence cleaned");  
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

MFEMDivFreeVolumetricSource::MFEMDivFreeVolumetricSource(const InputParameters & parameters)
  : MFEMSource(parameters),
    _vec_coef(const_cast<MFEMVectorCoefficient *>(
        &getUserObject<MFEMVectorCoefficient>("vector_coefficient"))),
    sourcecoefs(blocks.size()),
    coilsegments(blocks.size()),
    source_coef_name(std::string("source_") + getParam<std::string>("_object_name")),
    hcurl_fespace(getUserObject<MFEMFESpace>("hcurl_fespace")),
    h1_fespace(getUserObject<MFEMFESpace>("h1_fespace"))
{
  for (unsigned int i = 0; i < blocks.size(); i++)
  {
    sourcecoefs[i] = _vec_coef->getVectorCoefficient();
    coilsegments[i] = int(blocks[i]);
  }
  _restricted_coef = new mfem::PWVectorCoefficient(3, coilsegments, sourcecoefs);

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

  hephaestus::InputParameters div_free_source_params;
  div_free_source_params.SetParam("SourceName", source_coef_name);
  div_free_source_params.SetParam("HCurlFESpaceName", hcurl_fespace.name());
  div_free_source_params.SetParam("H1FESpaceName", h1_fespace.name());
  div_free_source_params.SetParam("SolverOptions", _solver_options);

  _source = new hephaestus::DivFreeSource(div_free_source_params);
}

hephaestus::Source *
MFEMDivFreeVolumetricSource::getSource()
{
  return _source;
}

void
MFEMDivFreeVolumetricSource::storeCoefficients(hephaestus::Coefficients & coefficients)
{
  coefficients.vectors.Register(source_coef_name, _restricted_coef, true);
}

MFEMDivFreeVolumetricSource::~MFEMDivFreeVolumetricSource() {}
