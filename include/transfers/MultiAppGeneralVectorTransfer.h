#pragma once

/**
 * Include headers of all MultiApp Transfers we would like.
 */
#include "MultiAppFieldTransfer.h"
#include "MultiAppTransfer.h"
#include "MultiAppCopyTransfer.h"
#include "MooseVariableFEBase.h"
#include "MultiAppGeneralFieldNearestLocationTransfer.h"
#include "Factory.h"

// Forwards declaration.
template <typename MultiAppTransferClassType>
class MultiAppVectorTransferTemplate;

/**
 * Create all transfer types using template here. Don't forget to register them in .C file.
 */
typedef MultiAppVectorTransferTemplate<MultiAppCopyTransfer> MultiAppGeneralVectorCopyTransfer;
typedef MultiAppVectorTransferTemplate<MultiAppGeneralFieldNearestLocationTransfer>
    MultiAppGeneralVectorNearestLocationTransfer;

template <typename MultiAppTransferClassType>
class MultiAppVectorTransferTemplate : public MultiAppTransferClassType
{
public:
  static InputParameters validParams();

  MultiAppVectorTransferTemplate(const InputParameters & parameters);

  void execute() override final;
  void initialSetup() override final;

  /**
   * Override so we can return modified variable names (removing vector variable names and replacing
   * with standard variable names for components).
   */
  std::vector<VariableName> getFromVarNames() const override final;
  std::vector<AuxVariableName> getToVarNames() const override final;

protected:
  /**
   * An enumeration used internally to specify the component of a vector variable.
   */
  enum class VectorComponent
  {
    X,
    Y,
    Z
  };

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
   * TODO: - Edward: Add documentation here please.
   */
  void addVectorAuxKernel(FEProblemBase & problem,
                          std::string & vector_name,
                          VectorAuxKernelType vector_mode);

  /**
   * This method is called internally to locate all vector variables. For each vector variable it
   * locates, it will create standard scalar variables. When the methods getFromVarNames() and
   * getToVarNames() are called, they will return a vector containing only the standard variable
   * names.
   */
  void convertAllVariables();

  /**
   * This method will iterate through the variable names. If the variable associated with the name
   * happens to be a vector variable, the standard variables for its components will be created.
   * Returns a vector containing the updated varaible names (adding all new standard variables and
   * removing any vector variables).
   */
  template <class VariableNameClassType>
  std::vector<VariableNameClassType>
  convertVariables(FEProblemBase & problem,
                   std::vector<VariableNameClassType> & input_variable_names);

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

  MooseVariableFEBase & getStandardVariable(FEProblemBase & problem,
                                            std::string & variable_name) const;

  MooseVariableFEBase & getVectorVariable(FEProblemBase & problem,
                                          std::string & variable_name) const;

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

  bool _has_converted_variables;
  Factory & _factory;
};