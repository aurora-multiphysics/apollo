#include "MFEMSource.h"
#include "Function.h"

registerMooseObject("ApolloApp", MFEMSource);

InputParameters
MFEMSource::validParams()
{
  InputParameters params = GeneralUserObject::validParams();
  params.addParam<FunctionName>(
      "function", 0, "The vector function providing the source, to be divergence cleaned.");
  params.addParam<std::vector<SubdomainID>>(
      "block", "The list of blocks (ids or names) that this object will be applied");
  return params;
}

MFEMSource::MFEMSource(const InputParameters & parameters)
  : GeneralUserObject(parameters),
    blocks(getParam<std::vector<SubdomainID>>("block")),
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
    coilsegments(blocks.size())
{
  for (unsigned int i = 0; i < blocks.size(); i++)
  {
    sourcecoefs[i] = &_vec_function_coef;
    coilsegments[i] = int(blocks[i]);
  }
  _restricted_coef = new mfem::PWVectorCoefficient(3, coilsegments, sourcecoefs);

  hephaestus::InputParameters div_free_source_params;
  div_free_source_params.SetParam("SourceName", std::string("source"));
  div_free_source_params.SetParam("HCurlFESpaceName", std::string("_HCurlFESpace"));
  div_free_source_params.SetParam("H1FESpaceName", std::string("_H1FESpace"));
  _source = new hephaestus::DivFreeVolumetricSource(div_free_source_params);
}

hephaestus::Source *
MFEMSource::getSource()
{
  return _source;
}

void
MFEMSource::storeCoefficients(hephaestus::DomainProperties & domain_properties)
{
  std::string coef_name = std::string("Source");
  domain_properties.vector_property_map["unblockedsource"] = &_vec_function_coef;
  domain_properties.vector_property_map["source"] = _restricted_coef;
}

MFEMSource::~MFEMSource() {}
