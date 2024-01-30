#pragma once

#include "MFEMMaterial.h"
#include "coefficients.hpp"

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
  const MFEMCoefficient * _electrical_conductivity_coeff{nullptr};
  const MFEMCoefficient * _permittivity_coeff{nullptr};
  const MFEMCoefficient * _permeability_coeff{nullptr};
};
