#pragma once

#include "MFEMMaterial.h"
#include "materials.hpp"

class MFEMConductor : public MFEMMaterial
{
public:
  static InputParameters validParams();

  MFEMConductor(const InputParameters & parameters);
  virtual ~MFEMConductor();

  /// EM constants
  static const Real _mu0;
  static const Real _epsilon0;

  void storeCoefficients(hephaestus::Subdomain & subdomain) override;

protected:
  const mfem::Coefficient & _electrical_conductivity_coeff;
  const mfem::Coefficient & _permittivity_coeff;
  const mfem::Coefficient & _permeability_coeff;
  const mfem::Coefficient & _heat_capacity_coeff;
  const mfem::Coefficient & _thermal_conductivity_coeff;
};
