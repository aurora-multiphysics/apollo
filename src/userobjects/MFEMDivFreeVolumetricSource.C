#include "MFEMDivFreeVolumetricSource.h"

registerMooseObject("ApolloApp", MFEMDivFreeVolumetricSource);

InputParameters
MFEMDivFreeVolumetricSource::validParams()
{
  InputParameters params = MFEMSource::validParams();
  params.addParam<FunctionName>(
      "function", "The vector function providing the source, to be divergence cleaned.");
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
    _func(getFunction("function")),
    _vec_function_coef(3,
                       [&](const mfem::Vector & p, double t, mfem::Vector & u) {
                         libMesh::RealVectorValue vector_value =
                             _func.vectorValue(t, PointFromMFEMVector(p));
                         u[0] = vector_value(0);
                         u[1] = vector_value(1);
                         u[2] = vector_value(2);
                       }),
    sourcecoefs(blocks.size()),
    coilsegments(blocks.size()),
    source_coef_name(std::string("source_") + getParam<std::string>("_object_name"))
{
  for (unsigned int i = 0; i < blocks.size(); i++)
  {
    sourcecoefs[i] = &_vec_function_coef;
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
  div_free_source_params.SetParam("HCurlFESpaceName", std::string("_HCurlFESpace"));
  div_free_source_params.SetParam("H1FESpaceName", std::string("_H1FESpace"));
  div_free_source_params.SetParam("SolverOptions", _solver_options);

  _source = new hephaestus::DivFreeSource(div_free_source_params);
}

hephaestus::Source *
MFEMDivFreeVolumetricSource::getSource()
{
  return _source;
}

void
MFEMDivFreeVolumetricSource::storeCoefficients(hephaestus::DomainProperties & domain_properties)
{
  domain_properties.vector_property_map[source_coef_name] = _restricted_coef;
}

MFEMDivFreeVolumetricSource::~MFEMDivFreeVolumetricSource() {}
