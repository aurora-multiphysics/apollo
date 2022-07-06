//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once
#include "ExclusiveMFEMMesh.h"

/**
 * CoupledMFEMMesh
 */
class CoupledMFEMMesh : public ExclusiveMFEMMesh {
 public:
  static InputParameters validParams();

  CoupledMFEMMesh(const InputParameters& parameters);

  virtual void buildMesh() override;

  // Builds the corresponding MOOSE mesh
  void buildRealMesh();

  // Function that maps vtk element types to libmesh element types
  int map_elems_vtk_to_libmesh(int VTKElemType);

  // Boolean value determining whether the user wants the corresponding MOOSE
  // mesh built
  bool CreateMOOSEMesh;

  virtual std::unique_ptr<MooseMesh> safeClone() const override;
};
