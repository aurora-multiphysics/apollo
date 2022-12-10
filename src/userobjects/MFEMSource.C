//* Convert into Action; see INSAction for example.
#include "MFEMSource.h"

registerMooseObject("ApolloApp", MFEMSource);

static void
source_current(const mfem::Vector & xv, double t, mfem::Vector & J)
{
  double x0(194e-3);  // Coil centre x coordinate
  double y0(100e-3);  // Coil centre y coordinate
  double a(50e-3);    // Coil thickness
  double I0(2742);    // Coil current in Ampere-turns
  double S(2.5e-3);   // Coil cross sectional area
  double freq(200.0); // Frequency in Hz

  double Jmag = (I0 / S) * sin(2 * M_PI * freq * t);

  double x = xv(0);
  double y = xv(1);
  if (abs(x - x0) < a)
  {
    J(0) = -(y - y0) / abs(y - y0);
  }
  else if (abs(y - y0) < a)
  {
    J(0) = 0.0;
  }
  else
  {
    J(0) = -(y - (y0 + a * ((y - y0) / abs(y - y0)))) /
           hypot(x - (x0 + a * ((x - x0) / abs(x - x0))), y - (y0 + a * ((y - y0) / abs(y - y0))));
  }

  if (abs(y - y0) < a)
  {
    J(1) = (x - x0) / abs(x - x0);
  }
  else if (abs(x - x0) < a)
  {
    J(1) = 0.0;
  }
  else
  {
    J(1) = (x - (x0 + a * ((x - x0) / abs(x - x0)))) /
           hypot(x - (x0 + a * ((x - x0) / abs(x - x0))), y - (y0 + a * ((y - y0) / abs(y - y0))));
  }

  // J(0) = 0.0;
  // // sin(M_PI * x(1)) * sin(M_PI * x(2)) * (t * 2 * M_PI * M_PI + 1);
  // J(1) = 0.0;
  J(2) = 0.0;
  J *= Jmag;
}

InputParameters
MFEMSource::validParams()
{
  InputParameters params = GeneralUserObject::validParams();
  return params;
}

MFEMSource::MFEMSource(const InputParameters & parameters) : GeneralUserObject(parameters)
{

  JSrcCoef = new mfem::VectorFunctionCoefficient(3, source_current);
  mfem::Array<mfem::VectorCoefficient *> sourcecoefs(4);
  sourcecoefs[0] = JSrcCoef;
  sourcecoefs[1] = JSrcCoef;
  sourcecoefs[2] = JSrcCoef;
  sourcecoefs[3] = JSrcCoef;
  mfem::Array<int> coilsegments(4);
  coilsegments[0] = 3;
  coilsegments[1] = 4;
  coilsegments[2] = 5;
  coilsegments[3] = 6;
  JSrcRestricted = new mfem::PWVectorCoefficient(3, coilsegments, sourcecoefs);
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
  domain_properties.vector_property_map["unblockedsource"] = JSrcCoef;
  domain_properties.vector_property_map["source"] = JSrcRestricted;
}

MFEMSource::~MFEMSource() {}
