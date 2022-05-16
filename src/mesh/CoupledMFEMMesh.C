//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "CoupledMFEMMesh.h"
#include "libmesh/face_quad4.h"

registerMooseObject("ApolloApp", CoupledMFEMMesh);

InputParameters CoupledMFEMMesh::validParams() {
  InputParameters params = ExclusiveMFEMMesh::validParams();
  return params;
}

CoupledMFEMMesh::CoupledMFEMMesh(const InputParameters& parameters)
    : MooseMesh(parameters),
      mfem_mesh((std::string)getParam<MeshFileName>("file"))
{
  
}

void CoupledMFEMMesh::buildMesh() {
  if (CreateMOOSEMesh) {
    _console << "Creating the MOOSE mesh" << std::endl;
    buildRealMesh();
  } else {
    _console << "Not creating a corresponding MOOSE mesh, this is an MFEM only "
                "problem"
             << std::endl;
    buildDummyMesh();
  }
}

void CoupledMFEMMesh::buildDummyMesh() {
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

void CoupledMFEMMesh::buildRealMesh() {
  mfem_mesh.get_connectivity_data();
  mfem_mesh.get_mesh_nodes();
  mfem_mesh.get_cell_type();

  libmesh_assert_equal_to(_mesh->processor_id(), 0);

  for (unsigned int i = 0; i < (mfem_mesh.pointsVec.size() / 3); i++) {
    double pnt[3];
    pnt[0] = mfem_mesh.pointsVec[i * 3];
    pnt[1] = mfem_mesh.pointsVec[(i * 3) + 1];
    pnt[2] = mfem_mesh.pointsVec[(i * 3) + 2];
    Point xyz(pnt[0], pnt[1], pnt[2]);

    _mesh->add_point(xyz, i);
  }

  int vertCounter = 0;
  for (unsigned int i = 0; i < mfem_mesh.cellTypeVec.size(); i++) {
    ElemType libmesh_elem_type =
        (ElemType)map_elems_vtk_to_libmesh(mfem_mesh.cellTypeVec[i]);
    Elem* elem = Elem::build(libmesh_elem_type).release();

    for (unsigned int j = 0; j < mfem_mesh.verticesVec[i]; j++) {
      elem->set_node(j) =
          _mesh->node_ptr(mfem_mesh.connectivityVec[vertCounter + j]);
    }

    vertCounter += mfem_mesh.verticesVec[i];

    std::vector<dof_id_type> conn;
    elem->connectivity(0, VTK, conn);

    // Below comment from original libmesh implementation, not quite sure what
    // it means right now, but it works ¯\_(ツ)_/¯.
    // Needs further investigation

    // then reshuffle the nodes according to the connectivity, this
    // two-time-assign would evade the definition of the vtk_mapping
    for (unsigned int j = 0, n_conn = cast_int<unsigned int>(conn.size());
         j != n_conn; ++j) {
      elem->set_node(j) = _mesh->node_ptr(conn[j]);
    }

    elem->set_id(i);

    _mesh->add_elem(elem);
  }  // end loop over VTK cells
  _mesh->set_mesh_dimension(dim);
  _mesh->prepare_for_use();
}

int CoupledMFEMMesh::map_elems_vtk_to_libmesh(int VTKElemType) {
  std::map<int, int> VTKtolibElemMap;
  VTKtolibElemMap[1] = 0;
  VTKtolibElemMap[21] = 1;
  VTKtolibElemMap[5] = 3;
  VTKtolibElemMap[22] = 4;
  VTKtolibElemMap[9] = 5;
  VTKtolibElemMap[23] = 6;
  VTKtolibElemMap[10] = 8;
  VTKtolibElemMap[24] = 9;
  VTKtolibElemMap[12] = 10;
  VTKtolibElemMap[25] = 11;
  VTKtolibElemMap[29] = 12;
  VTKtolibElemMap[13] = 13;
  VTKtolibElemMap[26] = 14;
  VTKtolibElemMap[32] = 15;
  VTKtolibElemMap[14] = 16;
  return VTKtolibElemMap[VTKElemType];
}

std::unique_ptr<MooseMesh> CoupledMFEMMesh::safeClone() const {
  return std::make_unique<CoupledMFEMMesh>(*this);
}