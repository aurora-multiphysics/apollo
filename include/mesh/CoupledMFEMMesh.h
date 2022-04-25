//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once
#include "MFEMMesh.h"
#include "MooseMesh.h"
#include "libmesh/elem.h"
#include "libmesh/enum_io_package.h"
#include "libmesh/equation_systems.h"
#include "libmesh/ignore_warnings.h"
#include "libmesh/libmesh_config.h"
#include "libmesh/mesh_base.h"
#include "libmesh/mesh_input.h"
#include "libmesh/node.h"
#include "libmesh/numeric_vector.h"
#include "libmesh/system.h"
#include "libmesh/vtk_io.h"
#include "mfem.hpp"

/**
 * CoupledMFEMMesh
 */
class CoupledMFEMMesh : public MooseMesh {
 public:
  static InputParameters validParams();

  CoupledMFEMMesh(const InputParameters& parameters);

  virtual void buildMesh() override;

  // Builds placeholder mesh when no MOOSE mesh is required
  void buildDummyMesh();
  // Builds the corresponding MOOSE mesh
  void buildRealMesh();

  // Function that maps vtk element types to libmesh element types
  int map_elems_vtk_to_libmesh(int VTKElemType);

  // Mesh dimension
  int dim;

  // The object holding our MFEM mesh. Needs renaming as to avoid confusion
<<<<<<< HEAD
  MFEMMesh mfem_mesh;
=======
  MFEMMesh other_mesh;
>>>>>>> d6e58f9 (Updated mesh src and include file names)

  // Boolean value determining whether the user wants the corresponding MOOSE
  // mesh built
  bool CreateMOOSEMesh;

  virtual std::unique_ptr<MooseMesh> safeClone() const override;
};
