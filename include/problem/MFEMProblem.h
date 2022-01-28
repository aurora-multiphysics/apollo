# pragma once

#include "ExternalProblem.h"

#include<iostream>


class MFEMProblem : public ExternalProblem
{
 public:
  static InputParameters validParams();

  MFEMProblem(const InputParameters & params);

  virtual void externalSolve () override;

  virtual bool converged () override { return true; };

  virtual void syncSolutions(Direction direction) override {};
 protected:
 std::string _problem_type;
 std::string _input_mesh;
};