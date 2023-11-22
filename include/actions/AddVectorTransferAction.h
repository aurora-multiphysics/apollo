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
  void convertAllVariables();

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

  bool isPushTransfer() const;
  bool isPullTransfer() const;
  bool isSupportedTransfer() const;

  bool isSupportedVectorVariable(MooseVariableFEBase & variable) const;
  bool isSupportedComponentVariable(MooseVariableFEBase & variable) const;
  bool areCompatibleVariables(MooseVariableFEBase & vector_variable,
                              MooseVariableFEBase & component_variable) const;

  template <class VariableNameClassType>
  std::vector<VariableNameClassType>
  convertVariables(FEProblemBase & problem,
                   std::vector<VariableNameClassType> & input_variable_names);

  std::string buildVectorComponentExtension(VectorComponent component) const;
  InputParameters buildInputParametersForComponents(MooseVariableFEBase & vector_variable) const;

  /**
   * Returns components of the enum class. This allows iterating over the components.
   */
  inline std::array<VectorComponent, 3> getAllComponents() const
  {

    std::array<VectorComponent, 3> components = {
        VectorComponent::X, VectorComponent::Y, VectorComponent::Z};

    return components;
  }

  /**
   * Creates the name of the variable corresponding to a component of a vector variable.
   */
  inline std::string buildVectorComponentName(const std::string vector_name,
                                              VectorComponent component) const
  {
    return (vector_name + buildVectorComponentExtension(component));
  }

private:
  std::vector<VariableName> _from_var_names_converted;
  std::vector<AuxVariableName> _to_var_names_converted;

  /**
   * MultiApps the transfer is transferring data to or from.
   */
  std::shared_ptr<MultiApp> _from_multi_app;
  std::shared_ptr<MultiApp> _to_multi_app;

  bool _has_converted_variables;
};
