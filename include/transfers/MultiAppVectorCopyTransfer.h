#pragma once

#include "MultiAppVectorDofCopyTransfer.h"
#include "MooseVariableFEBase.h"

/**
 * Copy the fields directly from one application to another.
 *
 * Similar to "MultiAppDofCopyTransfer" but for vectors. This class
 * should not be called directly.
 */
class MultiAppVectorCopyTransfer : public MultiAppVectorDofCopyTransfer
{
public:
  static InputParameters validParams();

  MultiAppVectorCopyTransfer(const InputParameters & parameters);

  /**
   * Performs the transfer of a vector variable to/from the Multiapp.
   */
  virtual void execute() override;

protected:
  virtual std::vector<VariableName> getFromVarNames() const override { return _from_var_names; }
  virtual std::vector<AuxVariableName> getToVarNames() const override { return _to_var_names; }

  const std::vector<VariableName> _from_var_names;
  const std::vector<AuxVariableName> _to_var_names;
};
