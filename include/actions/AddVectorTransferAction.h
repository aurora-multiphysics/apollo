#pragma once

#include "MooseObjectAction.h"

class AddVectorTransferAction : public MooseObjectAction
{
public:
  static InputParameters validParams();

  AddVectorTransferAction(const InputParameters & params);

  virtual void act() override;

protected:
  const std::vector<VariableName> & getFromVarNames() const;
  const std::vector<AuxVariableName> & getToVarNames() const;

  /**
   * Returns a shared pointer to or from the multiapp.
   */
  const std::shared_ptr<MultiApp> getFromMultiApp() const;
  const std::shared_ptr<MultiApp> getToMultiApp() const;

  /**
   * Finds the FEProblemBase depending on the direction.
   */
  FEProblemBase & getFromProblem() const;
  FEProblemBase & getToProblem() const;

  MooseVariableFEBase &
  getVariable(FEProblemBase & problem, std::string & variable_name, Moose::VarFieldType type) const;

  MooseVariableFEBase & getStandardVariable(FEProblemBase & problem,
                                            std::string & variable_name) const;
  MooseVariableFEBase & getVectorVariable(FEProblemBase & problem,
                                          std::string & variable_name) const;
  void initializeMultiApps();

private:
  /**
   * An enumeration used internally to specify the component of a vector variable.
   */
  enum class VectorComponent
  {
    X,
    Y,
    Z
  };

  /**
   * An enumeration used to specify which auxkernel to build.
   */
  enum class VectorAuxKernelType
  {
    PREPARE_VECTOR_FOR_TRANSFER,
    RECOVER_VECTOR_POST_TRANSFER
  };

  /**
   * MultiApps the transfer is transferring data to or from.
   */
  std::shared_ptr<MultiApp> _from_multi_app;
  std::shared_ptr<MultiApp> _to_multi_app;
};
