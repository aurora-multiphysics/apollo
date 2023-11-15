#pragma once

#include "MultiAppVectorDofCopyTransfer.h"
#include "MooseVariableFEBase.h"

/**
 * Similar to "MultiAppCopyTransfer" but inherits from MultiAppVectorDofCopyTransfer. This enables
 * vector transfer between a master app and a subapp.
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
