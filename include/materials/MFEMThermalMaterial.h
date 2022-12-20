#pragma once

#include "MFEMMaterial.h"
#include "materials.hpp"

class MFEMThermalMaterial : public MFEMMaterial
{
public:
  static InputParameters validParams();

  MFEMThermalMaterial(const InputParameters & parameters);
  virtual ~MFEMThermalMaterial();

  void storeCoefficients(hephaestus::Subdomain & subdomain) override;

protected:
  const mfem::Coefficient & _heat_capacity_coeff;
  const mfem::Coefficient & _thermal_conductivity_coeff;
};
