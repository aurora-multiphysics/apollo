# pragma once

#include "ExternalProblem.h"
#include "MFEMMesh.h"
#include "MFEMMaterial.h"
#include "MFEMBoundaryCondition.h"
#include "MFEMDataCollection.h"
#include "Function.h"
#include "libmesh/point.h"
#include "boundary_conditions.hpp"
#include "materials.hpp"
#include "executioner.hpp"
#include "outputs.hpp"
#include "auxiliary_variables.hpp"

#include "libmesh/string_to_enum.h"
#include "MooseEnum.h"
#include "AuxiliarySystem.h"
#include "DisplacedProblem.h"

#include<iostream>


class MFEMProblem : public ExternalProblem
{
 public:
  static InputParameters validParams();

  MFEMProblem(const InputParameters & params);

  virtual void externalSolve () override;

  virtual bool converged () override { return true; };

  virtual void syncSolutions(Direction direction) override;

  void addBoundaryCondition(const std::string & bc_name,
                            const std::string & name,
                            InputParameters & parameters) override;

  void addMaterial(const std::string & kernel_name,
                   const std::string & name,
                   InputParameters & parameters);


protected:
  std::string _input_mesh;
  std::string _formulation;
  int _order;
  hephaestus::BCMap _bc_maps;
  hephaestus::DomainProperties _mat_map;
  hephaestus::Executioner _executioner;
  hephaestus::Outputs _outputs;
};
