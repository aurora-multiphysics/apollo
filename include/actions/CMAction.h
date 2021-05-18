#pragma once

#include "Action.h"
#include "MooseEnum.h"
#include "libmesh/fe_type.h"

/**
 * This class allows us to have a section of the input file like the
 * following which automatically adds variables, kernels, aux kernels, bcs
 * for setting up the full wave complex Maxwell equations.
 *
 * [ComplexMaxwell]
 * []
 */
class CMAction : public Action
{
public:
  static InputParameters validParams();

  CMAction(InputParameters parameters);

  virtual void act() override;

protected:
  void addCMKernels();
  void addCMPECBC();
  void addCMWaveguidePortsBC();
  /// FE type for variables
  FEType _fe_type;

  /// Boundaries at which perfect electrical
  /// conductor conditions are applied (E꜀×n = 0)
  std::vector<BoundaryName> _pec_boundaries;
  Real _pec_penalty;

  /// (Rectangular) waveguide input and output ports.
  std::vector<BoundaryName> _wg_input_ports;
  std::vector<BoundaryName> _wg_output_ports;
};
