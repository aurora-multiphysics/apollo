#pragma once

#include "MFEMMaterial.h"
#include "coefficients.hpp"

class MFEMThermalMaterial : public MFEMMaterial
{
public:
  static InputParameters validParams();

  MFEMThermalMaterial(const InputParameters & parameters);
  virtual ~MFEMThermalMaterial();

  void storeCoefficients(hephaestus::Subdomain & subdomain) override;

protected:
  const MFEMCoefficient * _heat_capacity_coeff{nullptr};
  const MFEMCoefficient * _thermal_conductivity_coeff{nullptr};
};
