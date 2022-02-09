# pragma once

#include "ExternalProblem.h"
#include "MFEMInputs.h"
#include "boundary_conditions.hpp"

#include<iostream>


class MFEMProblem : public ExternalProblem
{
 public:
  static InputParameters validParams();

  MFEMProblem(const InputParameters & params);

  virtual void externalSolve () override;

  virtual bool converged () override { return true; };

  virtual void syncSolutions(Direction direction) override {};

  void addBoundaryCondition(const std::string & bc_name,
                            const std::string & name,
                            InputParameters & parameters) override;

  void addMaterial(const std::string & kernel_name,
                   const std::string & name,
                   InputParameters & parameters);

  // MFEMInputs _mfem_inputs;

 protected:
 std::string _problem_type;
 std::string _input_mesh;
 std::vector<BCMap> _bc_maps;

};