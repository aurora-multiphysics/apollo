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
#include <chrono>
#include <unistd.h>

registerMooseObject("ApolloApp", CoupledMFEMMesh);

InputParameters CoupledMFEMMesh::validParams() {
  InputParameters params = ExclusiveMFEMMesh::validParams();
  return params;
}

CoupledMFEMMesh::CoupledMFEMMesh(const InputParameters& parameters)
    : ExclusiveMFEMMesh(parameters)
{
}

CoupledMFEMMesh::~CoupledMFEMMesh() {}

void CoupledMFEMMesh::buildMesh() {
  //Use method from file mesh to build MOOSE mesh from exodus file
  FileMesh::buildMesh();
  //Create MFEM Mesh
  std::cout << "Checkpoint 1: MOOSE mesh created" << std::endl;
  auto start = std::chrono::steady_clock::now();
  createMFEMMesh();
  auto end = std::chrono::steady_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
  std::cout << "Time taken for MFEM mesh creation = " << elapsed << std::endl;
}

std::unique_ptr<MooseMesh> CoupledMFEMMesh::safeClone() const {
  return std::make_unique<CoupledMFEMMesh>(*this);
}

int CoupledMFEMMesh::getNumSidesets() {
  libMesh::BoundaryInfo& bdInf = getMesh().get_boundary_info();
  const std::set<boundary_id_type>& ss = bdInf.get_side_boundary_ids();
  return ss.size();
}

void CoupledMFEMMesh::getBdrLists(int** elem_ss, int** side_ss) {

  bdrElems = _bnd_elems.size();
  
  std::vector<dof_id_type> element_id_list(bdrElems, 0);
  std::vector<unsigned short int> side_list(bdrElems, 0);
  std::vector<BoundaryID> bc_id_list(bdrElems, 0);
  std::vector<int> sides_in_ss_holder(num_side_sets, 0);

  for (int i = 0; i < _bnd_elems.size(); i++) {
    element_id_list[i] = _bnd_elems[i]->_elem->id();
    side_list[i] = _bnd_elems[i]->_side;
    bc_id_list[i] = _bnd_elems[i]->_bnd_id;
  }

  for (int i = 0; i < bc_id_list.size(); i++) {
    sides_in_ss_holder[bc_id_list[i] - 1]++;
  }

  num_sides_in_ss = sides_in_ss_holder;

  for (int i = 0; i < num_side_sets; i++) {
    elem_ss[i] = new int[num_sides_in_ss[i]];
    side_ss[i] = new int[num_sides_in_ss[i]];
  }

  for (int bc_id = 0; bc_id <= num_side_sets; bc_id++) {
    int counter = 0;
    for (int side = 0; side < bdrElems; side++) {
      if ((bc_id + 1) == bc_id_list[side]) {
        elem_ss[bc_id][counter] = element_id_list[side];
        side_ss[bc_id][counter] = side_list[side];
        counter++;
      }
    }
  }
}

void CoupledMFEMMesh::getElementInfo() {
  const Elem* elementPtr = elemPtr(0);
  num_node_per_el = elementPtr->n_nodes();

  if (_dim == 2) {
    switch (num_node_per_el) {
      case (3): {
        libmesh_element_type = ELEMENT_TRI3;
        libmesh_face_type = FACE_EDGE2;
        num_element_linear_nodes = 3;
        break;
      }
      case (6): {
        libmesh_element_type = ELEMENT_TRI6;
        libmesh_face_type = FACE_EDGE3;
        num_element_linear_nodes = 3;
        break;
      }
      case (4): {
        libmesh_element_type = ELEMENT_QUAD4;
        libmesh_face_type = FACE_EDGE2;
        num_element_linear_nodes = 4;
        break;
      }
      case (9): {
        libmesh_element_type = ELEMENT_QUAD9;
        libmesh_face_type = FACE_EDGE3;
        num_element_linear_nodes = 4;
        break;
      }
      default: {
        MFEM_ABORT("Don't know what to do with a " << num_node_per_el
                                                   << " node 2D element\n");
      }
    }
  } 
  else if (_dim == 3) {
    switch (num_node_per_el) {
      case (4): {
        libmesh_element_type = ELEMENT_TET4;
        libmesh_face_type = FACE_TRI3;
        num_element_linear_nodes = 4;
        break;
      }
      case (10): {
        libmesh_element_type = ELEMENT_TET10;
        libmesh_face_type = FACE_TRI6;
        num_element_linear_nodes = 4;
        break;
      }
      case (8): {
        libmesh_element_type = ELEMENT_HEX8;
        libmesh_face_type = FACE_QUAD4;
        num_element_linear_nodes = 8;
        break;
      }
      case (27): {
        libmesh_element_type = ELEMENT_HEX27;
        libmesh_face_type = FACE_QUAD9;
        num_element_linear_nodes = 8;
        break;
      }
      default: {
        MFEM_ABORT("Don't know what to do with a " << num_node_per_el
                                                   << " node 3D element\n");
      }
    }
  }

  switch (libmesh_face_type) {
    case (FACE_EDGE2): {
      num_face_nodes = 2;
      num_face_linear_nodes = 2;
      break;
    }
    case (FACE_EDGE3): {
      num_face_nodes = 3;
      num_face_linear_nodes = 2;
      break;
    }
    case (FACE_TRI3): {
      num_face_nodes = 3;
      num_face_linear_nodes = 3;
      break;
    }
    case (FACE_TRI6): {
      num_face_nodes = 6;
      num_face_linear_nodes = 3;
      break;
    }
    case (FACE_QUAD4): {
      num_face_nodes = 4;
      num_face_linear_nodes = 4;
      break;
    }
    case (FACE_QUAD9): {
      num_face_nodes = 9;
      num_face_linear_nodes = 4;
      break;
    }
  }
}

void CoupledMFEMMesh::createMFEMMesh() {
  //These are all maps that enable us to get the vertices on 
  //one side of the mesh using the indexing system of [side number][node of that side]
  const int sideMapTri3[3][2] = {
      {1, 2},
      {2, 3},
      {3, 1},
  };

  const int sideMapQuad4[4][2] = {
      {1, 2},
      {2, 3},
      {3, 4},
      {4, 1},
  };

  const int sideMapTri6[3][3] = {
      {1, 2, 4},
      {2, 3, 5},
      {3, 1, 6},
  };

  const int sideMapQuad9[4][3] = {
      {1, 2, 5},
      {2, 3, 6},
      {3, 4, 7},
      {4, 1, 8},
  };

  const int sideMapTet4[4][3] = {{1, 3, 2}, {1, 2, 4}, {2, 3, 4}, {1, 4, 3}};

  const int sideMapTet10[4][6] = {{1, 2, 4, 5, 9, 8},
                                  {2, 3, 4, 6, 10, 9},
                                  {1, 4, 3, 8, 10, 7},
                                  {1, 3, 2, 7, 6, 5}};

  const int sideMapHex8[6][4] = {{1, 2, 6, 5}, {2, 3, 7, 6}, {4, 3, 7, 8},
                                 {1, 4, 8, 5}, {1, 4, 3, 2}, {5, 8, 7, 6}};

  const int sideMapHex27[6][9] = {
      {1, 2, 6, 5, 9, 14, 17, 13, 26},  {2, 3, 7, 6, 10, 15, 18, 14, 25},
      {4, 3, 7, 8, 11, 15, 19, 16, 27}, {1, 4, 8, 5, 12, 16, 20, 13, 24},
      {1, 4, 3, 2, 12, 11, 10, 9, 22},  {5, 8, 7, 6, 20, 19, 18, 17, 23}};

  const int mfemToGenesisTet10[10] = {1, 2, 3, 4, 5, 7, 8, 6, 9, 10};

  //                                  1,2,3,4,5,6,7,8,9,10,11,
  const int mfemToGenesisHex27[27] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                                      // 12,13,14,15,16,17,18,19
                                      12, 17, 18, 19, 20, 13, 14, 15,
                                      // 20,21,22,23,24,25,26,27
                                      16, 22, 26, 25, 27, 24, 23, 21};

  const int mfemToGenesisTri6[6] = {1, 2, 3, 4, 5, 6};
  const int mfemToGenesisQuad9[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

  buildBndElemList();

  getElementInfo();

  num_side_sets = getNumSidesets();
  int** elem_ss = new int*[num_side_sets];
  int** side_ss = new int*[num_side_sets];

  getBdrLists(elem_ss, side_ss);

  std::set<subdomain_id_type> block_ids;
  getMesh().subdomain_ids(block_ids);
  int num_el_blk = (int)(getMesh().n_subdomains());
  std::map<subdomain_id_type, std::string> id_to_name_map =
      getMesh().get_subdomain_name_map();

  size_t* num_el_in_blk = new size_t[num_el_blk];
  int* start_of_block = new int[num_el_blk + 1];

  int* ebprop = new int[num_el_blk];
  int* ssprop = new int[num_side_sets];

  for (int i = 0; i < num_el_blk; i++) {
    ebprop[i] = i + 1;
  }
  for (int i = 0; i < num_side_sets; i++) {
    ssprop[i] = i + 1;
  }
  
  for (int block : block_ids) {
    int num_el_in_block_counter = 1;
    for (libMesh::MeshBase::element_iterator el_ptr =
             getMesh().active_subdomain_elements_begin(block);
         el_ptr != getMesh().active_subdomain_elements_end(block); el_ptr++) {
      num_el_in_blk[block - 1] = num_el_in_block_counter++;
      
    }
  }

  // elem_blk is a 2D array that stores all the nodes of all the elements in all
  // the blocks Indexing is done as so, elem_blk[block_id][node]
  int** elem_blk = new int*[num_el_blk];
  for (int i = 0; i < (int)num_el_blk; i++) {
    elem_blk[i] = new int[num_el_in_blk[i] * num_node_per_el];
  }

  // Here we are setting all the values in elem_blk
  for (int block : block_ids) {
    int elem_count = 0;
    for (libMesh::MeshBase::element_iterator el_ptr =
             getMesh().active_subdomain_elements_begin(block);
         el_ptr != getMesh().active_subdomain_elements_end(block); el_ptr++) {
      for (int el_nodes = 0; el_nodes < num_node_per_el; el_nodes++) {
        elem_blk[block - 1][(elem_count * num_node_per_el) + el_nodes] =
            (*el_ptr)->node_id(el_nodes);
      }
      elem_count++;
    }
  }
  
  start_of_block[0] = 0;
  for (int i = 1; i < (int)num_el_blk + 1; i++) {
    start_of_block[i] = start_of_block[i - 1] + num_el_in_blk[i - 1];
  }

  int** ss_node_id = new int*[num_side_sets];
  create_ss_node_id(elem_ss, side_ss, ss_node_id);  

  std::vector<int> uniqueVertexID;
  for (int iblk = 0; iblk < (int)num_el_blk; iblk++) {
    for (int i = 0; i < (int)num_el_in_blk[iblk]; i++) {
      for (int j = 0; j < num_element_linear_nodes; j++) {
        uniqueVertexID.push_back(1 + elem_blk[iblk][i * num_node_per_el + j]);
      }
    }
  }

  std::sort(uniqueVertexID.begin(), uniqueVertexID.end());
  std::vector<int>::iterator newEnd;
  newEnd = std::unique(uniqueVertexID.begin(), uniqueVertexID.end());
  uniqueVertexID.resize(std::distance(uniqueVertexID.begin(), newEnd));

  // OK at this point uniqueVertexID contains a list of all the nodes that are
  // actually used by the mesh, 1-based, and sorted. We need to invert this
  // list, the inverse is a map

  std::map<int, int> cubitToMFEMVertMap;
  for (int i = 0; i < (int)uniqueVertexID.size(); i++) {
    cubitToMFEMVertMap[uniqueVertexID[i]] = i + 1;
  }

  std::vector<double> coordx(nNodes(), 0);
  std::vector<double> coordy(nNodes(), 0);
  std::vector<double> coordz(nNodes(), 0);
  int node_counter = 0;
  for (auto i = localNodesBegin(); i != localNodesEnd(); i++) {
    coordx[node_counter] = (**i)(0);
    coordy[node_counter] = (**i)(1);
    coordz[node_counter] = (**i)(2);
    node_counter++;
  }
  
  int num_elem = nElem();
  mfem_mesh = new MFEMMesh(num_elem, coordx, coordy, coordz, cubitToMFEMVertMap, uniqueVertexID,
      libmesh_element_type, libmesh_face_type, elem_blk, num_el_blk,
      num_node_per_el, num_el_in_blk, num_element_linear_nodes, num_face_nodes,
      num_face_linear_nodes, num_side_sets, num_sides_in_ss, ss_node_id, ebprop,
      ssprop, 3, start_of_block);

  delete[] elem_ss;
  delete[] side_ss;
  delete[] ss_node_id;
}

void CoupledMFEMMesh::create_ss_node_id(int** elem_ss, int** side_ss, int** ss_node_id)
{
  for (int i = 0; i < (int)num_side_sets; i++) { 
    ss_node_id[i] = new int[num_sides_in_ss[i] * num_face_nodes];
    for (int j = 0; j < (int)num_sides_in_ss[i]; j++) {
      int glob_ind = elem_ss[i][j];
      int side = side_ss[i][j];
      Elem* elem = elemPtr(glob_ind);
      std::vector<unsigned int> nodes = elem->nodes_on_side(side);
      for(int k = 0; k<num_face_nodes; k++)
      {
        ss_node_id[i][j * num_face_nodes + k] = elem->node_id(nodes[k]);
      }
    }
  }  
}