//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ExclusiveMFEMMesh.h"
#include "libmesh/face_quad4.h"

registerMooseObject("MooseApp", ExclusiveMFEMMesh);

InputParameters
ExclusiveMFEMMesh::validParams()
{
  InputParameters params = FileMesh::validParams();
  return params;
}

ExclusiveMFEMMesh::ExclusiveMFEMMesh(const InputParameters & parameters) : FileMesh(parameters) {}

ExclusiveMFEMMesh::~ExclusiveMFEMMesh() {}

void
ExclusiveMFEMMesh::buildMesh()
{
  buildDummyMesh();
  mfem_mesh = std::make_shared<MFEMMesh>(getFileName());
  MFEMParMesh = new mfem::ParMesh(MPI_COMM_WORLD, *mfem_mesh);
}

void
ExclusiveMFEMMesh::buildDummyMesh()
{
  int e = 1;
  auto elem = new Quad4;
  elem->set_id() = e;
  elem->processor_id() = 0;
  _mesh->add_elem(elem);

  Point pt1(0.0, 0.0, 0.0);
  Point pt2(1.0, 0.0, 0.0);
  Point pt3(1.0, 1.0, 0.0);
  Point pt4(0.0, 1.0, 0.0);

  elem->set_node(0) = _mesh->add_point(pt1);
  elem->set_node(1) = _mesh->add_point(pt2);
  elem->set_node(2) = _mesh->add_point(pt3);
  elem->set_node(3) = _mesh->add_point(pt4);

  _mesh->prepare_for_use();
}

std::unique_ptr<MooseMesh>
ExclusiveMFEMMesh::safeClone() const
{
  return std::make_unique<ExclusiveMFEMMesh>(*this);
}
