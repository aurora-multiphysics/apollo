#pragma once

#include "MultiAppFieldTransfer.h"
#include "MooseVariableFEBase.h"

// Forwards declaration.
namespace libMesh
{
class DofObject;
}

/**
 * Copy the fields directly from one application to another.
 *
 * Similar to "MultiAppDofCopyTransfer" but for vectors. This class
 * should not be called directly.
 */
class MultiAppVectorDofCopyTransfer : public MultiAppFieldTransfer
{
public:
  static InputParameters validParams();

  MultiAppVectorDofCopyTransfer(const InputParameters & parameters);

protected:
  /**
   * Performs the transfer of a variable between two problems if they have the same mesh.
   */
  void transfer(FEProblemBase & to_problem, FEProblemBase & from_problem);

  /**
   * Performs the transfer of values between a node or element.
   */
  void transferVectorDofs(libMesh::DofObject & to_object,
                          libMesh::DofObject & from_object,
                          MooseVariableFieldBase & to_var,
                          MooseVariableFieldBase & from_var,
                          NumericVector<Number> & to_solutin,
                          NumericVector<Number> & from_solution);

  /**
   * Checks vector variable is of correct family and order.
   */
  bool isSupportedVectorVariable(MooseVariableFEBase & variable) const;
};
