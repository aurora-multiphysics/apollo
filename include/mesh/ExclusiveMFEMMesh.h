//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once
#include "FileMesh.h"
#include "MFEMMesh.h"
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
class ExclusiveMFEMMesh : public FileMesh {
 public:
  static InputParameters validParams();

  ExclusiveMFEMMesh(const InputParameters& parameters);

  virtual ~ExclusiveMFEMMesh();

  virtual std::unique_ptr<MooseMesh> safeClone() const override;

  virtual void buildMesh() override;

  // Builds placeholder mesh when no MOOSE mesh is required
  void buildDummyMesh();

  // The object holding our MFEM mesh. Needs renaming as to avoid confusion
  MFEMMesh* mfem_mesh;
};