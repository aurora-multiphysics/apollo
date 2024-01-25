#include "MFEMRWTE10PortRBC.h"

registerMooseObject("ApolloApp", MFEMRWTE10PortRBC);

InputParameters
MFEMRWTE10PortRBC::validParams()
{
  InputParameters params = MFEMBoundaryCondition::validParams();
  params.addRequiredParam<RealVectorValue>(
      "port_length_vector",
      "The vector directed along the longest length of a rectangular waveguide port,"
      "with magnitude equal to the length of this edge in m ");
  params.addRequiredParam<RealVectorValue>(
      "port_width_vector",
      "The vector directed along the shortest length of a rectangular waveguide port,"
      "with magnitude equal to the length of this edge in m ");
  params.addRequiredParam<Real>("frequency", "The input frequency in Hz.");
  params.addRequiredParam<bool>("input_port",
                                "Set to true to specify that the direction of propogation of the "
                                "EM wave at the port is into the domain");
  return params;
}

// TODO: Currently assumes the vector function coefficient is 3D
MFEMRWTE10PortRBC::MFEMRWTE10PortRBC(const InputParameters & parameters)
  : MFEMBoundaryCondition(parameters),
    _frequency(getParam<Real>("frequency")),
    _port_length_vector(getParam<RealVectorValue>("port_length_vector")),
    _port_width_vector(getParam<RealVectorValue>("port_width_vector")),
    _lv{_port_length_vector(0), _port_length_vector(1), _port_length_vector(2)},
    _wv{_port_width_vector(0), _port_width_vector(1), _port_width_vector(2)},
    _input_port(getParam<bool>("input_port"))
{
  _boundary_condition = std::make_shared<hephaestus::RWTE10PortRBC>(
      getParam<std::string>("variable"), bdr_attr, _frequency, _lv, _wv, _input_port);
}
