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

InputParameters
CoupledMFEMMesh::validParams()
{
  InputParameters params = ExclusiveMFEMMesh::validParams();
  return params;
}

CoupledMFEMMesh::CoupledMFEMMesh(const InputParameters & parameters) : ExclusiveMFEMMesh(parameters)
{
}

CoupledMFEMMesh::~CoupledMFEMMesh() {}

void
CoupledMFEMMesh::buildMesh()
{
  // Use method from file mesh to build MOOSE mesh from Exodus file.
  FileMesh::buildMesh();
}

std::unique_ptr<MooseMesh>
CoupledMFEMMesh::safeClone() const
{
  return std::make_unique<CoupledMFEMMesh>(*this);
}

int
CoupledMFEMMesh::getNumSidesets()
{
  libMesh::BoundaryInfo & bdInf = getMesh().get_boundary_info();
  const std::set<boundary_id_type> & ss = bdInf.get_side_boundary_ids();
  return ss.size();
}

void
CoupledMFEMMesh::getBdrLists(int ** elem_ss, int ** side_ss)
{
  bdrElems = _bnd_elems.size();

  std::vector<dof_id_type> element_id_list(bdrElems, 0);
  std::vector<unsigned short int> side_list(bdrElems, 0);
  std::vector<BoundaryID> bc_id_list(bdrElems, 0);
  std::vector<int> sides_in_ss_holder(num_side_sets, 0);

  for (int i = 0; i < _bnd_elems.size(); i++)
  {
    element_id_list[i] = _bnd_elems[i]->_elem->id();
    side_list[i] = _bnd_elems[i]->_side;
    bc_id_list[i] = _bnd_elems[i]->_bnd_id;
  }

  for (std::size_t i = 0; i < bc_id_list.size(); i++)
  {
    sides_in_ss_holder[bc_id_list[i] - 1]++;
  }

  num_sides_in_ss = sides_in_ss_holder;

  for (int i = 0; i < num_side_sets; i++)
  {
    elem_ss[i] = new int[num_sides_in_ss[i]];
    side_ss[i] = new int[num_sides_in_ss[i]];
  }

  for (int bc_id = 0; bc_id <= num_side_sets; bc_id++)
  {
    int counter = 0;
    for (int side = 0; side < bdrElems; side++)
    {
      if ((bc_id + 1) == bc_id_list[side])
      {
        elem_ss[bc_id][counter] = element_id_list[side];
        side_ss[bc_id][counter] = side_list[side];
        counter++;
      }
    }
  }
}

void
CoupledMFEMMesh::getElementInfo()
{
  const Elem * element_ptr = elemPtr(0);
  num_node_per_el = element_ptr->n_nodes();

  if (_dim == 2)
  {
    switch (num_node_per_el)
    {
      case (3):
      {
        libmesh_element_type = ELEMENT_TRI3;
        libmesh_face_type = FACE_EDGE2;
        num_element_linear_nodes = 3;
        break;
      }
      case (6):
      {
        libmesh_element_type = ELEMENT_TRI6;
        libmesh_face_type = FACE_EDGE3;
        num_element_linear_nodes = 3;
        break;
      }
      case (4):
      {
        libmesh_element_type = ELEMENT_QUAD4;
        libmesh_face_type = FACE_EDGE2;
        num_element_linear_nodes = 4;
        break;
      }
      case (9):
      {
        libmesh_element_type = ELEMENT_QUAD9;
        libmesh_face_type = FACE_EDGE3;
        num_element_linear_nodes = 4;
        break;
      }
      default:
      {
        MFEM_ABORT("Don't know what to do with a " << num_node_per_el << " node 2D element\n");
      }
    }
  }
  else if (_dim == 3)
  {
    switch (num_node_per_el)
    {
      case (4):
      {
        libmesh_element_type = ELEMENT_TET4;
        libmesh_face_type = FACE_TRI3;
        num_element_linear_nodes = 4;
        break;
      }
      case (10):
      {
        libmesh_element_type = ELEMENT_TET10;
        libmesh_face_type = FACE_TRI6;
        num_element_linear_nodes = 4;
        break;
      }
      case (8):
      {
        libmesh_element_type = ELEMENT_HEX8;
        libmesh_face_type = FACE_QUAD4;
        num_element_linear_nodes = 8;
        break;
      }
      case (27):
      {
        libmesh_element_type = ELEMENT_HEX27;
        libmesh_face_type = FACE_QUAD9;
        num_element_linear_nodes = 8;
        break;
      }
      default:
      {
        MFEM_ABORT("Don't know what to do with a " << num_node_per_el << " node 3D element\n");
      }
    }
  }

  switch (libmesh_face_type)
  {
    case (FACE_EDGE2):
    {
      num_face_nodes = 2;
      num_face_linear_nodes = 2;
      break;
    }
    case (FACE_EDGE3):
    {
      num_face_nodes = 3;
      num_face_linear_nodes = 2;
      break;
    }
    case (FACE_TRI3):
    {
      num_face_nodes = 3;
      num_face_linear_nodes = 3;
      break;
    }
    case (FACE_TRI6):
    {
      num_face_nodes = 6;
      num_face_linear_nodes = 3;
      break;
    }
    case (FACE_QUAD4):
    {
      num_face_nodes = 4;
      num_face_linear_nodes = 4;
      break;
    }
    case (FACE_QUAD9):
    {
      num_face_nodes = 9;
      num_face_linear_nodes = 4;
      break;
    }
    default:
    {
      break;
    }
  }
}

void
CoupledMFEMMesh::buildMFEMMesh()
{
  // These are all maps that enable us to get the vertices on
  // one side of the mesh using the indexing system of [side number][node of that side]
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

  const int sideMapTet10[4][6] = {
      {1, 2, 4, 5, 9, 8}, {2, 3, 4, 6, 10, 9}, {1, 4, 3, 8, 10, 7}, {1, 3, 2, 7, 6, 5}};

  const int sideMapHex8[6][4] = {
      {1, 2, 6, 5}, {2, 3, 7, 6}, {4, 3, 7, 8}, {1, 4, 8, 5}, {1, 4, 3, 2}, {5, 8, 7, 6}};

  const int sideMapHex27[6][9] = {{1, 2, 6, 5, 9, 14, 17, 13, 26},
                                  {2, 3, 7, 6, 10, 15, 18, 14, 25},
                                  {4, 3, 7, 8, 11, 15, 19, 16, 27},
                                  {1, 4, 8, 5, 12, 16, 20, 13, 24},
                                  {1, 4, 3, 2, 12, 11, 10, 9, 22},
                                  {5, 8, 7, 6, 20, 19, 18, 17, 23}};

  buildBndElemList();

  // Retrieve information about the elements used within the mesh
  getElementInfo();

  // Elem_ss and side_ss store information about which elements are in each sideset, and which sides
  // of those elements are contained within the sideset
  num_side_sets = getNumSidesets();

  int ** elem_ss = new int *[num_side_sets];
  int ** side_ss = new int *[num_side_sets];

  // Populate the elem_ss and side_ss
  getBdrLists(elem_ss, side_ss);

  // block_ids
  std::set<subdomain_id_type> block_ids;
  getMesh().subdomain_ids(block_ids);

  int num_blocks_in_mesh = (int)(getMesh().n_subdomains());

  std::size_t * num_elements_per_block = new size_t[num_blocks_in_mesh];
  int * start_of_block = new int[num_blocks_in_mesh + 1];

  int * ebprop = new int[num_blocks_in_mesh];
  int * ssprop = new int[num_side_sets];

  for (int i = 0; i < num_blocks_in_mesh; i++)
  {
    ebprop[i] = i + 1;
  }

  for (int i = 0; i < num_side_sets; i++)
  {
    ssprop[i] = i + 1;
  }

  // Loops to set num_elements_per_block.
  for (int iblock : block_ids)
  {
    int num_elements_in_block_counter = 1;

    for (libMesh::MeshBase::element_iterator element_ptr =
             getMesh().active_subdomain_elements_begin(iblock);
         element_ptr != getMesh().active_subdomain_elements_end(iblock);
         element_ptr++)
    {
      num_elements_per_block[iblock - 1] = num_elements_in_block_counter++;
    }
  }

  // elem_blk is a 2D array that stores all the nodes of all the elements in all
  // the blocks. Indexing is done as so, elem_blk[block_id][node]
  int ** elem_blk = new int *[num_blocks_in_mesh];
  for (int i = 0; i < num_blocks_in_mesh; i++)
  {
    elem_blk[i] = new int[num_elements_per_block[i] * num_node_per_el];
  }

  // Here we are setting all the values in elem_blk
  for (int iblock : block_ids)
  {
    int elem_count = 0;
    for (libMesh::MeshBase::element_iterator el_ptr =
             getMesh().active_subdomain_elements_begin(iblock);
         el_ptr != getMesh().active_subdomain_elements_end(iblock);
         el_ptr++)
    {
      for (int el_nodes = 0; el_nodes < num_node_per_el; el_nodes++)
      {
        elem_blk[iblock - 1][(elem_count * num_node_per_el) + el_nodes] =
            (*el_ptr)->node_id(el_nodes);
      }
      elem_count++;
    }
  }

  // start_of_block is just an array of ints that represent what the first element id of
  // each block is
  start_of_block[0] = 0;
  for (int i = 1; i < num_blocks_in_mesh + 1; i++)
  {
    start_of_block[i] = start_of_block[i - 1] + num_elements_per_block[i - 1];
  }

  // ss_node_id stores all the id's of all the sides in a sideset
  // for example, ss_node_id[0][0] would access the first node id in the first sideset
  int ** ss_node_id = new int *[num_side_sets];
  createSidesetNodeIDs(elem_ss, side_ss, ss_node_id);

  std::vector<int> unique_vertex_ids;
  for (int iblock = 0; iblock < num_blocks_in_mesh; iblock++)
  {
    for (int jelement = 0; jelement < num_elements_per_block[iblock]; jelement++)
    {
      for (int knode = 0; knode < num_element_linear_nodes; knode++)
      {
        unique_vertex_ids.push_back(1 + elem_blk[iblock][jelement * num_node_per_el + knode]);
      }
    }
  }

  // Setting map
  std::sort(unique_vertex_ids.begin(), unique_vertex_ids.end());
  std::vector<int>::iterator newEnd;
  newEnd = std::unique(unique_vertex_ids.begin(), unique_vertex_ids.end());
  unique_vertex_ids.resize(std::distance(unique_vertex_ids.begin(), newEnd));

  // OK at this point unique_vertex_ids contains a list of all the nodes that are
  // actually used by the mesh, 1-based, and sorted. We need to invert this
  // list, the inverse is a map
  std::map<int, int> cubit_to_MFEM_vertex_map;
  for (std::size_t i = 0; i < unique_vertex_ids.size(); i++)
  {
    cubit_to_MFEM_vertex_map[unique_vertex_ids[i]] = i;
  }

  // for (auto& [key, value] : cubit_to_MFEM_vertex_map) {
  //   // value -= 1;
  //   std::cout << '[' << key << "] = " << value << "; " << std::endl;
  // }
  std::vector<double> coordx(nNodes(), 0);
  std::vector<double> coordy(nNodes(), 0);
  std::vector<double> coordz(nNodes(), 0);
  int node_counter = 0;

  // Populating coord data structures to hold all the node coordinates needed to create
  // the MFEM mesh
  // This could be problematic if localNodesBegin and End don't access nodes in ascending node id,
  // but this never seems to happen
  for (auto node : getMesh().node_ptr_range())
  {
    unsigned int node_id = node->id();

    coordx[node_id] = (*node)(0);
    coordy[node_id] = (*node)(1);
    coordz[node_id] = (*node)(2);
  }

  int num_elements_in_mesh = nElem();

  // Create MFEM mesh using this extremely long but necessary constructor
  _mfem_mesh = std::make_shared<MFEMMesh>(num_elements_in_mesh,
                                          coordx,
                                          coordy,
                                          coordz,
                                          cubit_to_MFEM_vertex_map,
                                          unique_vertex_ids,
                                          libmesh_element_type,
                                          libmesh_face_type,
                                          elem_blk,
                                          num_blocks_in_mesh,
                                          num_node_per_el,
                                          num_elements_per_block,
                                          num_element_linear_nodes,
                                          num_face_nodes,
                                          num_face_linear_nodes,
                                          num_side_sets,
                                          num_sides_in_ss,
                                          ss_node_id,
                                          ebprop,
                                          ssprop,
                                          3,
                                          start_of_block,
                                          libmeshToMFEMNode);

  // Memory cleanup.
  for (int i = 0; i < num_side_sets; i++)
  {
    delete[] ss_node_id[i];
    delete[] side_ss[i];
    delete[] elem_ss[i];
  }

  delete[] ss_node_id;
  delete[] side_ss;
  delete[] elem_ss;

  for (int i = 0; i < num_blocks_in_mesh; i++)
  {
    delete[] elem_blk[i];
  }

  delete[] elem_blk;

  delete[] num_elements_per_block;

  delete[] ebprop;
  delete[] ssprop;
  delete[] start_of_block;
}

/**
 * Returns the libMesh partitioning. The "raw" pointer is wrapped up in a unique
 * pointer.
 */
std::unique_ptr<int[]>
CoupledMFEMMesh::getMeshPartitioning() const
{
  const MeshBase & lib_mesh = getMesh();

  const int num_elements = lib_mesh.n_elem();
  if (num_elements < 1)
  {
    return nullptr;
  }

  int * mesh_partitioning = new int[num_elements];

  for (auto element : lib_mesh.element_ptr_range())
  {
    int element_id = element->id();

    mesh_partitioning[element_id] = element->processor_id();
  }

  // Wrap-up in a unique pointer.
  return std::unique_ptr<int[]>(mesh_partitioning);
}

void
CoupledMFEMMesh::buildMFEMParMesh()
{
  // int * partitioning = new int[getMesh().n_nodes()];
  // for (auto node : getMesh().node_ptr_range())
  // {
  //   unsigned int node_id = node->id();
  //   partitioning[node_id] = node->processor_id();
  // }

  _mfem_par_mesh = std::make_shared<MFEMParMesh>(MPI_COMM_WORLD, *getMFEMMesh());
  // _mfem_par_mesh = std::make_shared<MFEMParMesh>(MPI_COMM_WORLD, *getMFEMMesh(), partitoning);
}

void
CoupledMFEMMesh::createSidesetNodeIDs(int ** elem_ss, int ** side_ss, int ** ss_node_id)
{
  for (std::size_t i = 0; i < num_side_sets; i++)
  {
    ss_node_id[i] = new int[num_sides_in_ss[i] * num_face_nodes];

    for (std::size_t j = 0; j < num_sides_in_ss[i]; j++)
    {
      int glob_ind = elem_ss[i][j];
      int side = side_ss[i][j];

      Elem * elem = elemPtr(glob_ind);

      std::vector<unsigned int> nodes = elem->nodes_on_side(side);

      for (int k = 0; k < num_face_nodes; k++)
      {
        ss_node_id[i][j * num_face_nodes + k] = elem->node_id(nodes[k]);
      }
    }
  }
}
