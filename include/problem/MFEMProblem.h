#pragma once

#include <iostream>
#include <iostream>

#include "AuxiliarySystem.h"
#include "DisplacedProblem.h"
#include "ExternalProblem.h"
#include "Function.h"
#include "MFEMBoundaryCondition.h"
#include "MFEMMaterial.h"
#include "MFEMMesh.h"
#include "MooseEnum.h"
#include "auxiliary_variables.hpp"
#include "boundary_conditions.hpp"
#include "executioner.hpp"
#include "libmesh/string_to_enum.h"

class MFEMProblem : public ExternalProblem {
 public:
  static InputParameters validParams();

  MFEMProblem(const InputParameters& params);

  virtual void externalSolve() override;

  virtual bool converged() override { return true; };

  virtual void syncSolutions(Direction direction) override;

  void addBoundaryCondition(const std::string& bc_name, 
                            const std::string& name,
                            InputParameters& parameters) override;

  void addMaterial(const std::string& kernel_name, 
                   const std::string& name,
                   InputParameters& parameters);

  void addAuxVariable(const std::string& var_type, 
                      const std::string& var_name,
                      InputParameters& parameters);

  void setMFEMVarData(EquationSystems& esRef,
                      hephaestus::AuxiliaryVariable* var);

  void setMOOSEVarData(hephaestus::AuxiliaryVariable* var, 
                       EquationSystems& esRef);

  MFEMMesh& getMFEMMesh();

  mfem::FiniteElementCollection* fecGet(std::string var_fam);

 protected:
  std::string _input_mesh;
  std::string _formulation;
  int _order;
  hephaestus::BCMap _bc_maps;
  hephaestus::DomainProperties _mat_map;
  hephaestus::Executioner _executioner;
  hephaestus::AuxVarMap _var_map;
};
