# pragma once

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
<<<<<<< HEAD
#include "auxiliary_variables.hpp"

#include "libmesh/string_to_enum.h"
#include "MooseEnum.h"
#include "AuxiliarySystem.h"
#include "DisplacedProblem.h"

#include<iostream>


class MFEMProblem : public ExternalProblem
{
=======
#include "libmesh/point.h"
#include "libmesh/string_to_enum.h"
#include "materials.hpp"

class MFEMProblem : public ExternalProblem {
>>>>>>> 57f0f2774a00f8347794ec599c25d2ea8c35e050
 public:
  static InputParameters validParams();

  MFEMProblem(const InputParameters& params);

  virtual void externalSolve () override;

  virtual bool converged () override { return true; };

  virtual void syncSolutions(Direction direction) override;
<<<<<<< HEAD
=======

  void addBoundaryCondition(const std::string& bc_name, const std::string& name,
                            InputParameters& parameters) override;

  void addMaterial(const std::string& kernel_name, const std::string& name,
                   InputParameters& parameters);

  void addAuxVariable(const std::string& var_type, const std::string& var_name,
                      InputParameters& parameters);

  void setMFEMVarData(EquationSystems& esRef,
                      hephaestus::AuxiliaryVariable* var);

  void setMOOSEVarData(EquationSystems& esRef,
                       hephaestus::AuxiliaryVariable* var);
>>>>>>> 57f0f2774a00f8347794ec599c25d2ea8c35e050

  MFEMMesh& getMFEMMesh();

  mfem::FiniteElementCollection* fecGet(std::string var_fam);

<<<<<<< HEAD
<<<<<<< HEAD
protected:
=======
 protected:
>>>>>>> 57f0f2774a00f8347794ec599c25d2ea8c35e050
  std::string _input_mesh;
  std::string _formulation;
  int _order;
  hephaestus::BCMap _bc_maps;
  hephaestus::MaterialMap _mat_map;
  hephaestus::Executioner _executioner;
<<<<<<< HEAD
=======

  void addAuxVariable(const std::string& var_type, 
                      const std::string& var_name,
                      InputParameters& parameters);

  void setMFEMVarData(EquationSystems& esRef, 
                  hephaestus::AuxiliaryVariable* var);

  void setMOOSEVarData(EquationSystems& esRef, 
                  hephaestus::AuxiliaryVariable* var);                  

  MFEMMesh& getMFEMMesh();

  
//   fespace getFeSpace(std::string& str);

  mfem::FiniteElementCollection* fecGet(std::string var_fam);

 protected:
 //std::string _input_mesh;
 std::string _formulation;
 int _order;
 hephaestus::BCMap _bc_maps;
 hephaestus::MaterialMap _mat_map;
 hephaestus::Executioner _executioner;
 hephaestus::AuxVarMap _var_map;
>>>>>>> b40ebb6 (Updated MFEMProblem to allow for variable creation and transfer of variable data)
=======
  hephaestus::AuxVarMap _var_map;
>>>>>>> 57f0f2774a00f8347794ec599c25d2ea8c35e050
};