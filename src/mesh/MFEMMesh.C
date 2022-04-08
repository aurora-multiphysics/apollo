//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "MFEMMesh.h"

registerMooseObject("MooseApp", MFEMMesh);

InputParameters
MFEMMesh::validParams()
{
  InputParameters params = GeneratedMesh::validParams();
  params.addRequiredParam<MeshFileName>("file", "The name of the mesh file to read");
  params.set<MooseEnum>("dim") = 3;
  return params;
}

MFEMMesh::MFEMMesh(const InputParameters & parameters)
  : GeneratedMesh(parameters), file_name(getParam<MeshFileName>("file"))
{
}