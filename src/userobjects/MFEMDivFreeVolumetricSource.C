#include "MFEMDivFreeVolumetricSource.h"

registerMooseObject("ApolloApp", MFEMDivFreeVolumetricSource);

InputParameters
MFEMDivFreeVolumetricSource::validParams()
{
  InputParameters params = MFEMSource::validParams();
  params.addParam<FunctionName>(
      "function", "The vector function providing the source, to be divergence cleaned.");
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

  hephaestus::InputParameters div_free_source_params;
  div_free_source_params.SetParam("SourceName", source_coef_name);
  div_free_source_params.SetParam("HCurlFESpaceName", std::string("_HCurlFESpace"));
  div_free_source_params.SetParam("H1FESpaceName", std::string("_H1FESpace"));
  _source = new hephaestus::DivFreeVolumetricSource(div_free_source_params);
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
