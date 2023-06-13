#include "RacetrackCoilCurrentDensity.h"

registerMooseObject("ApolloApp", RacetrackCoilCurrentDensity);

InputParameters
RacetrackCoilCurrentDensity::validParams()
{
  InputParameters params = Function::validParams();
  params.addClassDescription("A function that returns an AC current density flowing around a "
                             "racetrack coil, with coil axis in the z direction.");
  params.addRequiredParam<Real>("coil_axis_x", "The x coordinate of the coil axis in m");
  params.addRequiredParam<Real>("coil_axis_y", "The y coordinate of the coil axis in m");
  params.addRequiredParam<Real>("coil_thickness", "The coil thickness");
  params.addRequiredParam<Real>("coil_current_magnitude",
                                "The magnitude of the coil current in Ampere-turns");
  params.addRequiredParam<Real>("coil_xsection_area",
                                "Coil cross sectional area, in square metres.");
  params.addParam<Real>("frequency", 0.0, "Frequency of AC current in coil in Hz");
  return params;
}

RacetrackCoilCurrentDensity::RacetrackCoilCurrentDensity(const InputParameters & parameters)
  : Function(parameters),
    x0(getParam<Real>("coil_axis_x")),
    y0(getParam<Real>("coil_axis_y")),
    a(getParam<Real>("coil_thickness")),
    I0(getParam<Real>("coil_current_magnitude")),
    S(getParam<Real>("coil_xsection_area")),
    freq(getParam<Real>("frequency"))
{
}

RealVectorValue
RacetrackCoilCurrentDensity::vectorValue(Real t, const Point & p) const
{
  Real j_x, j_y, j_z; // Current density unit vector components
  double x = p(0);
  double y = p(1);

  Real Jmag; // Current density magnitude
  if (freq * t > 1e-15)
  {
    Jmag = (I0 / S) * sin(2 * M_PI * freq * t);
  }
  else
  {
    Jmag = (I0 / S);
  }
  // Calculate x component of current density unit vector
  if (abs(x - x0) < a)
  {
    j_x = -(y - y0) / abs(y - y0);
  }
  else if (abs(y - y0) < a)
  {
    j_x = 0.0;
  }
  else
  {
    j_x = -(y - (y0 + a * ((y - y0) / abs(y - y0)))) /
          hypot(x - (x0 + a * ((x - x0) / abs(x - x0))), y - (y0 + a * ((y - y0) / abs(y - y0))));
  }

  // Calculate y component of current density unit vector
  if (abs(y - y0) < a)
  {
    j_y = (x - x0) / abs(x - x0);
  }
  else if (abs(x - x0) < a)
  {
    j_y = 0.0;
  }
  else
  {
    j_y = (x - (x0 + a * ((x - x0) / abs(x - x0)))) /
          hypot(x - (x0 + a * ((x - x0) / abs(x - x0))), y - (y0 + a * ((y - y0) / abs(y - y0))));
  }

  // Calculate z component of current density unit vector
  j_z = 0.0;

  return Jmag * RealVectorValue(j_x, j_y, j_z);
}
