//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "LorentzForce.h"

registerMooseObject("ApolloApp", LorentzForce);

InputParameters
LorentzForce::validParams()
{
  InputParameters params = VectorAuxKernel::validParams();
  params.addRequiredCoupledVar("current_density", "The local current density (J).");
  params.addRequiredCoupledVar("magnetic_flux_density", "The flux density field (B).");
  return params;
}

LorentzForce::LorentzForce(const InputParameters & parameters)
  : VectorAuxKernel(parameters),
  _j_field(coupledVectorValue("current_density")),
  _b_field(coupledVectorValue("magnetic_flux_density"))
{
}

RealVectorValue
LorentzForce::computeValue()
{
  // Calculate the Lorentz force per unit volume.
  return _j_field[_qp].cross(_b_field[_qp]);
}
