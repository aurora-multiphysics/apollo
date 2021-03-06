#pragma once

#include "AuxiliarySystem.h"
#include "DisplacedProblem.h"
#include "ExternalProblem.h"
#include "ExclusiveMFEMMesh.h"
#include "MFEMMesh.h"
#include "MFEMMaterial.h"
#include "MFEMBoundaryCondition.h"
#include "MFEMDataCollection.h"
#include "Function.h"
#include "MooseEnum.h"
#include "auxiliary_variables.h"
#include "boundary_conditions.hpp"
#include "materials.hpp"
#include "executioner.hpp"
#include "outputs.hpp"
#include "libmesh/string_to_enum.h"
#include "libmesh/point.h"

class MFEMProblem : public ExternalProblem
{
public:
  static InputParameters validParams();

  MFEMProblem(const InputParameters & params);

  virtual void externalSolve() override;

  virtual bool converged() override { return true; };

  virtual void syncSolutions(Direction direction) override;

  /**
   * Overwritten mesh() method from base MooseMesh to retrieve the correct mesh type, in this case
   * ExclusiveMFEMMesh.
   */
  virtual ExclusiveMFEMMesh & mesh() override;

  /**
   * Returns all the variable names from the auxiliary system base. This is helpful in the
   * syncSolutions() method when transferring variable data.
   */
  virtual std::vector<VariableName> getAuxVariableNames();

  void addBoundaryCondition(const std::string & bc_name,
                            const std::string & name,
                            InputParameters & parameters) override;

  void addMaterial(const std::string & kernel_name,
                   const std::string & name,
                   InputParameters & parameters);
  /**
   * Override of ExternalProblem::addAuxVariable. Uses ExternalProblem::addAuxVariable to set the
   * Moose aux var, and contains additional code to create a corresponding MFEM grid function to be
   * used in the MFEM solve.
   */
  void addAuxVariable(const std::string & var_type,
                      const std::string & var_name,
                      InputParameters & parameters);
  /**
   * setMFEMVarData and setMOOSEVarData have very similar uses. They are both used to retrieve data
   * from one of the variable types (either Moose AuxVar or MFEM grid function), and transfer it to
   * the other. For example if you solve for temperature in MOOSE, you would use setMFEMVarData to
   * get this temperature data into an MFEM grid function.
   */
  void setMFEMVarData(EquationSystems & esRef, hephaestus::AuxiliaryVariable * var);
  /**
   * setMFEMVarData and setMOOSEVarData have very similar uses. They are both used to retrieve data
   * from one of the variable types (either Moose AuxVar or MFEM grid function), and transfer it to
   * the other. For example if you solve for temperature in MOOSE, you would use setMFEMVarData to
   * get this temperature data into an MFEM grid function.
   */
  void setMOOSEVarData(hephaestus::AuxiliaryVariable * var, EquationSystems & esRef);

  /**
   * Method used to get an mfem FEC depending on the variable family specified in the input file.
   * This method is used in addAuxVariable to help create the MFEM grid function that corresponds to
   * a given MOOSE aux-variable.
   */
  mfem::FiniteElementCollection * fecGet(std::string var_fam);

protected:
  std::string _input_mesh;
  std::string _formulation;
  int _order;
  hephaestus::BCMap _bc_maps;
  hephaestus::DomainProperties _domain_properties;
  hephaestus::Executioner _executioner;
  hephaestus::AuxVarMap _var_map;
  hephaestus::Outputs _outputs;
};
