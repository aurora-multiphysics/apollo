#pragma once

#include "Action.h"
#include "MooseEnum.h"
#include "libmesh/fe_type.h"

/**
 * This class allows us to have a section of the input file like the
 * following which automatically adds variables, kernels, aux kernels, bcs
 * for setting up the AV formulation.
 *
 * [AVFormulation]
 * []
 */
class AVAction : public Action
{
public:
  static InputParameters validParams();

  AVAction(InputParameters parameters);

  virtual void act() override;

protected:
  void addAVKernels();
  void addZeroFluxBC();
  void addTangentialHBC();
  void addElectricPotentialBC();
  void addElectricCurrentBC();

  /// FE types for variables
  FEType _vector_fe_type;
  FEType _scalar_fe_type;

  /// PMC boundaries at which tangential H fields are specified
  /// Imposes Neumann conditions on A.
  std::vector<BoundaryName> _tangent_h_boundaries;
  Real _tangent_h_penalty;
  std::vector<FunctionName> _surface_h_fields;

  /// PEC boundaries at which no magnetic flux passes through the surface.
  /// Imposes Dirichlet conditions on A
  std::vector<BoundaryName> _zero_flux_boundaries;
  Real _zero_flux_penalty;

  /// Boundaries held at fixed electrostatic potential
  /// Imposes Dirichlet condition on V.
  std::vector<BoundaryName> _electric_potential_boundaries;
  std::vector<Real> _surface_electric_potentials;

  /// Boundaries over which the local current density across them is fixed.
  /// Imposes Neumann condition on V.
  std::vector<BoundaryName> _electric_current_boundaries;
  std::vector<FunctionName> _surface_electric_currents;
};
