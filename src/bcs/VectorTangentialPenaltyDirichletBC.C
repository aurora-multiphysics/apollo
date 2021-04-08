//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
//*
//* Tangential component of vector u = -n×n×u
//* Adds to residual the term
//* penalty*<n×n×(u-u_0),v>
//*
//* Used when imposing the Robin boundary condition
//* <n×∇×u,v> - p<n×n×(u-u_0),v>= 0
//*
//* When the penalty p is large, this imposes a penalty-based Dirichlet BC:
//* p*<n×n×(u-u_0),v> = -p*<(u-u_0)×n,v×n> = 0
#include "VectorTangentialPenaltyDirichletBC.h"
#include "Function.h"

registerMooseObject("ApolloApp", VectorTangentialPenaltyDirichletBC);

InputParameters
VectorTangentialPenaltyDirichletBC::validParams()
{
  InputParameters params = VectorIntegratedBC::validParams();
  params.addRequiredParam<Real>("penalty", "The penalty coefficient");
  params.addParam<FunctionName>("function",
                                "The boundary condition vector function of the tangential component."
                                "This cannot be supplied with the component parameters.");
  params.addParam<FunctionName>("function_x", 0, "The function for the x component");
  params.addParam<FunctionName>("function_y", 0, "The function for the y component");
  params.addParam<FunctionName>("function_z", 0, "The function for the z component");
  params.addClassDescription(
      "Enforces a Dirichlet boundary condition on the tangential components of a vector "
      "at a surface in a weak sense with $p<n×n×(\\vec{u} - \\vec{u}_0), \\vec{v}>$, where: "
      "$p$ is the constant scalar penalty; "
      "$n$ is the surface normal vector; "
      "$\\vec{v}$ is the test function; "
      "and $\\vec{u} - \\vec{u}_0$ is the difference "
      "between the current solution and the Dirichlet data.");
  return params;
}

VectorTangentialPenaltyDirichletBC::VectorTangentialPenaltyDirichletBC(const InputParameters & parameters)
  : VectorIntegratedBC(parameters),
    _penalty(getParam<Real>("penalty")),
    _function(isParamValid("function") ? &getFunction("function") : nullptr),
    _function_x(getFunction("function_x")),
    _function_y(getFunction("function_y")),
    _function_z(getFunction("function_z"))
{
  if (_function && parameters.isParamSetByUser("function_x"))
    paramError("function_x", "The 'function' and 'function_x' parameters cannot both be set.");
  if (_function && parameters.isParamSetByUser("function_y"))
    paramError("function_y", "The 'function' and 'function_y' parameters cannot both be set.");
  if (_function && parameters.isParamSetByUser("function_z"))
    paramError("function_z", "The 'function' and 'function_z' parameters cannot both be set.");
}

Real
VectorTangentialPenaltyDirichletBC::computeQpResidual()
{
  RealVectorValue u_0;
  if (_function)
    u_0 = _function->vectorValue(_t, _q_point[_qp]);
  else
    u_0 = {_function_x.value(_t, _q_point[_qp]),
           _function_y.value(_t, _q_point[_qp]),
           _function_z.value(_t, _q_point[_qp])};
  RealVectorValue n_cross_u = (_u[_qp] - u_0).cross(_normals[_qp]);
  return _penalty * n_cross_u * ((_test[_i][_qp]).cross(_normals[_qp]));
}

Real
VectorTangentialPenaltyDirichletBC::computeQpJacobian()
{
  return _penalty * (_phi[_j][_qp]).cross(_normals[_qp]) * (_test[_i][_qp]).cross(_normals[_qp]);
}
