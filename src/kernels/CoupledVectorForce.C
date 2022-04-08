#include "CoupledVectorForce.h"

#include "MooseVariable.h"

registerMooseObject("MooseApp", CoupledVectorForce);

InputParameters
CoupledVectorForce::validParams()
{
  InputParameters params = VectorKernel::validParams();

  params.addClassDescription("Implements a source term proportional to the value of a coupled "
                             "vector variable. Weak form: $(\\psi_i, -\\sigma v)$.");
  params.addRequiredCoupledVar("v", "The coupled variable which provides the force");
  params.addParam<Real>(
      "coef", 1.0, "Coefficent ($\\sigma$) multiplier for the coupled force term.");

  return params;
}

CoupledVectorForce::CoupledVectorForce(const InputParameters & parameters)
  : VectorKernel(parameters),
    _v_var(coupled("v")),
    _v(coupledVectorValue("v")),
    _coef(getParam<Real>("coef"))
{
  if (_var.number() == _v_var)
    mooseError(
        "Coupled variable 'v' needs to be different from 'variable' with CoupledVectorForce, "
        "consider using Reaction or somethig similar");
}

Real
CoupledVectorForce::computeQpResidual()
{
  return -_coef * _v[_qp].contract(_test[_i][_qp]);
}

Real
CoupledVectorForce::computeQpJacobian()
{
  return 0;
}

Real
CoupledVectorForce::computeQpOffDiagJacobian(unsigned int jvar)
{
  if (jvar == _v_var)
    return -_coef * _phi[_j][_qp] * _test[_i][_qp];
  return 0.0;
}
