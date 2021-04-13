//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "Action.h"
#include "MooseEnum.h"
#include "libmesh/fe_type.h"

/**
 * This class allows us to have a section of the input file like the
 * following which automatically adds variables, kernels, aux kernels, bcs
 * for setting up the incompressible Navier-Stokes equation.
 *
 * [IncompressibleNavierStokes]
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
  /// FE type for various variables
  FEType _fe_type;
  /// Boundaries at which perfect electrical
  /// conductor conditions are applied
  std::vector<BoundaryName> _pec_boundaries;
  /// Penalty term for penalty-based Dirichlet methods at PEC boundaries.
  Real _pec_penalty;
  std::vector<BoundaryName> _wg_input_ports;
  std::vector<BoundaryName> _wg_output_ports;
};
