//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "GeneratedMesh.h"

class MFEMMesh;

template <>
InputParameters validParams<MFEMMesh>();

/**
 * MFEMMesh
 */
class MFEMMesh : public GeneratedMesh
{
public:
  static InputParameters validParams();

  MFEMMesh(const InputParameters & parameters);
  MFEMMesh(const MFEMMesh & other_mesh) = default;

public:
  /// the file_name from whence this mesh came
  std::string file_name;
};

