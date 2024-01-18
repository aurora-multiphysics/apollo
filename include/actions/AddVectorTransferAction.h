#pragma once

#include "MooseObjectAction.h"
#include "AuxiliarySystem.h"

class AddVectorTransferAction : public MooseObjectAction
{
public:
  static InputParameters validParams();

  AddVectorTransferAction(const InputParameters & params);

  virtual void act() override;

protected:
  /**
   * Returns a shared pointer to or from the multiapp.
   */
  const std::shared_ptr<MultiApp> getFromMultiApp() const;
  const std::shared_ptr<MultiApp> getToMultiApp() const;

  /**
   * Returns to/from problem variable names.
   */
  const std::vector<VariableName> & getFromVarNames() const;
  const std::vector<AuxVariableName> & getToVarNames() const;

  const std::vector<VariableName> & getFromVarNamesConverted();
  const std::vector<AuxVariableName> & getToVarNamesConverted();

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
   * An enumeration used to specify the auxkernel to build.
   */
  enum class VectorAuxKernelType
  {
    PREPARE_VECTOR_FOR_TRANSFER,
    RECOVER_VECTOR_POST_TRANSFER
  };

  /**
   * Finds the FEProblemBase depending on the direction.
   */
  FEProblemBase & getFromProblem() const;
  FEProblemBase & getToProblem() const;

  /**
   * Returns references to the auxsystem.
   */
  AuxiliarySystem & getFromAuxSystem() const;
  AuxiliarySystem & getToAuxSystem() const;

  /**
   * Adds a vector auxkernel to the problem.
   */
  void
  addVectorAuxKernel(FEProblemBase & problem, std::string & vector_name, VectorAuxKernelType type);

  /**
   * This method is called internally to locate all vector variables. For each vector variable
   * it locates, it will create standard scalar variables. When the methods getFromVarNames()
   * and getToVarNames() are called, they will return a vector containing only the standard
   * variable names.
   */
  void convertAllVariables();

  /**
   * This method will iterate through the variable names. If the variable associated with the name
   * happens to be a vector variable, the standard variables for its components will be created.
   * Returns a vector containing the updated variable names (adding all new standard variables and
   * removing any vector variables).
   */
  template <class VariableNameClassType>
  std::vector<VariableNameClassType>
  convertVariables(FEProblemBase & problem,
                   std::vector<VariableNameClassType> & input_variable_names);

  /**
   * This method verifies that source_type is a valid parameter. If it is and there is at least one
   * vector variable supplied then we need to duplicate its source type for the new standard
   * variable components.
   */
  void convertSourceTypes();

  /**
   * Returns true is "source_type" is a valid parameter.
   */
  bool hasSourceTypesParameter() const;

  /**
   * Returns a reference to a vector of converted source types.
   */
  std::vector<MooseEnum> & getSourceTypeConverted();

  /**
   * Returns all vector source variable names.
   */
  std::set<std::string> & getVectorSourceNames();

  /**
   * Returns true if variable name corresponds to a source vector variable.
   */
  bool isSourceVectorVariable(const std::string & var_name);

  /**
   * Creates component standard variables for the x, y and z components of a vector variable.
   */
  void buildVectorComponents(FEProblemBase & problem, MooseVariableFEBase & vector_variable);

  /**
   * Returns the extension for a given vector component.
   */
  std::string buildVectorComponentExtension(VectorComponent component) const;
  /**
   * Creates the FEType of a variable corresponding to a component of a vector variable.
   */
  InputParameters buildInputParametersForComponents(MooseVariableFEBase & vector_variable) const;

  /**
   * Check whether the variable is a vector variable of the supported family/order.
   */
  bool isSupportedVectorVariable(MooseVariableFEBase & vector_variable) const;

  /**
   * Check whether the component variable is of a valid family.
   */
  bool isSupportedComponentVariable(MooseVariableFEBase & variable) const;

  /**
   * Checks whether a vector variable is compatible with a component variable.
   */
  bool areCompatibleVariables(MooseVariableFEBase & vector_variable,
                              MooseVariableFEBase & component_variable) const;

  bool isPushTransfer() const;
  bool isPullTransfer() const;
  bool isSupportedTransfer() const;

  /**
   * Helper methods.
   */
  MooseVariableFEBase &
  getVariable(FEProblemBase & problem,
              std::string & variable_name,
              Moose::VarFieldType type = Moose::VarFieldType::VAR_FIELD_ANY) const;

  /**
   * Returns components of the enum class. This allows iterating over the components.
   */
  std::array<VectorComponent, 3> getAllComponents() const;

  /**
   * Creates the name of the variable corresponding to a component of a vector variable.
   */
  inline std::string buildVectorComponentName(const std::string & vector_name,
                                              VectorComponent component) const;

  /**
   * Creates the type name of the transfer suitable for vector variables.
   */
  inline std::string buildVectorTransferTypeName(const std::string & transfer_type) const;

  /**
   * Returns family/order of variable.
   */
  inline libMesh::FEFamily getVariableFamily(const MooseVariableFEBase & variable) const;
  inline libMesh::Order getVariableOrder(const MooseVariableFEBase & variable) const;

private:
  // Names of all source vector variables.
  std::set<std::string> _vector_source_names;

  // New variable names.
  std::vector<VariableName> _from_var_names_converted;
  std::vector<AuxVariableName> _to_var_names_converted;

  // New source types (if present).
  std::vector<MooseEnum> _source_type_converted;

  bool _has_converted_variables;
  bool _has_converted_source_types;
};

inline std::string
AddVectorTransferAction::buildVectorComponentName(const std::string & vector_name,
                                                  VectorComponent component) const
{
  return (vector_name + buildVectorComponentExtension(component));
}

inline std::string
AddVectorTransferAction::buildVectorTransferTypeName(const std::string & transfer_type) const
{
  return (transfer_type + "Vector");
}

inline std::array<AddVectorTransferAction::VectorComponent, 3>
AddVectorTransferAction::getAllComponents() const
{

  std::array<VectorComponent, 3> components = {
      VectorComponent::X, VectorComponent::Y, VectorComponent::Z};

  return components;
}

inline libMesh::FEFamily
AddVectorTransferAction::getVariableFamily(const MooseVariableFEBase & variable) const
{
  return variable.feType().family;
}

inline libMesh::Order
AddVectorTransferAction::getVariableOrder(const MooseVariableFEBase & variable) const
{
  return variable.order();
}