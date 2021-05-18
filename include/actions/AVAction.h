#pragma once

#include "Action.h"
#include "MooseEnum.h"
#include "libmesh/fe_type.h"

/**
 * This class allows us to have a section of the input file like the
 * following which automatically adds variables, kernels, aux kernels, bcs
 * for setting up the incompressible Navier-Stokes equation.
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
  /// FE type for various variables
  FEType _vector_fe_type;
  FEType _scalar_fe_type;
  /// Boundaries at which perfect electrical
  /// conductor conditions are applied
  std::vector<BoundaryName> _tangent_h_boundaries;
  Real _tangent_h_penalty;
  std::vector<FunctionName> _surface_h_fields;
  std::vector<BoundaryName> _zero_flux_boundaries;
  /// Penalty term for penalty-based Dirichlet methods at PEC boundaries.
  Real _zero_flux_penalty;
  std::vector<BoundaryName> _electric_potential_boundaries;
  std::vector<Real> _surface_electric_potentials;
  std::vector<BoundaryName> _electric_current_boundaries;
  std::vector<FunctionName> _surface_electric_currents;
};
