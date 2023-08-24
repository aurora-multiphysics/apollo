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
CoupledMFEMMesh::getBdrLists(std::vector<std::vector<int>> & elem_ss,
                             std::vector<std::vector<int>> & side_ss)
{
  buildBndElemList();

  _num_sides_in_ss = std::vector<int>(_num_sidesets, 0);

  struct BoundaryElementAndSideIDs
  {
    std::vector<int> element_ids; // Element ids for a boundary id.
    std::vector<int> side_ids;    // Side ids for a boundary id.

    BoundaryElementAndSideIDs() : element_ids{}, side_ids{} {}
  };

  std::vector<BoundaryID> unique_boundary_ids;
  std::map<BoundaryID, BoundaryElementAndSideIDs> boundary_ids_map;

  // Iterate over elements on the boundary to build the map that allows us to go
  // from a boundary id to a vector of element id/side ids.
  for (auto boundary_element : _bnd_elems)
  {
    auto boundary_id = boundary_element->_bnd_id;

    bool is_new_boundary_id = (boundary_ids_map.count(boundary_id) == 0);

    if (is_new_boundary_id) // Initialize new struct.
    {
      boundary_ids_map[boundary_id] = BoundaryElementAndSideIDs();
      unique_boundary_ids.push_back(boundary_id);
    }

    auto element_id = boundary_element->_elem->id(); // ID of element on boundary.
    auto side_id = boundary_element->_side;          // ID of side that element is on.

    boundary_ids_map[boundary_id].element_ids.push_back(element_id);
    boundary_ids_map[boundary_id].side_ids.push_back(side_id);
  }

  // Check that the boundary IDs are 1-based and are numbered sequentially.
  std::sort(unique_boundary_ids.begin(), unique_boundary_ids.end());

  if (!unique_boundary_ids.empty())
  {
    if (unique_boundary_ids.front() != 1)
      mooseError("Boundary IDs should be 1-based!");
    else if (unique_boundary_ids.back() != _num_sidesets)
      mooseError("Number of side sets does not match highest boundary ID.");
  }

  for (int i = 1; i < unique_boundary_ids.size(); i++)
  {
    if (unique_boundary_ids[i] != (unique_boundary_ids[i - 1] + 1))
    {
      mooseError("Boundary IDs should be numbered sequentially!");
      break;
    }
  }

  // Run through the (key, value) pairs in the boundary_ids_map map.
  for (const auto & key_value_pair : boundary_ids_map)
  {
    auto boundary_id = key_value_pair.first;

    auto element_ids = key_value_pair.second.element_ids;
    auto side_ids = key_value_pair.second.side_ids;

    // NB: subtract 1 as indices are 1-based.
    _num_sides_in_ss[boundary_id - 1] = element_ids.size();

    elem_ss[boundary_id - 1] = element_ids;
    side_ss[boundary_id - 1] = side_ids;
  }
}

void
CoupledMFEMMesh::buildLibmesh2DElementInfo()
{
  // TODO: - this will not work with distributed. Can we just get the first local element?
  const Elem * element_ptr = elemPtr(0);

  _num_nodes_per_element = element_ptr->n_nodes();

  switch (_num_nodes_per_element)
  {
    case 3:
    {
      _libmesh_element_type = ELEMENT_TRI3;
      _libmesh_face_type = FACE_EDGE2;
      _num_linear_nodes_per_element = 3;
      break;
    }
    case 6:
    {
      _libmesh_element_type = ELEMENT_TRI6;
      _libmesh_face_type = FACE_EDGE3;
      _num_linear_nodes_per_element = 3;
      break;
    }
    case 4:
    {
      _libmesh_element_type = ELEMENT_QUAD4;
      _libmesh_face_type = FACE_EDGE2;
      _num_linear_nodes_per_element = 4;
      break;
    }
    case 9:
    {
      _libmesh_element_type = ELEMENT_QUAD9;
      _libmesh_face_type = FACE_EDGE3;
      _num_linear_nodes_per_element = 4;
      break;
    }
    default:
    {
      mooseError("Invalid number of nodes (", _num_nodes_per_element, ") for a 2D element.");
      break;
    }
  }
}

void
CoupledMFEMMesh::buildLibmesh3DElementInfo()
{
  // TODO: - this will not work with distributed. Can we just get the first local element?
  const Elem * element_ptr = elemPtr(0);

  _num_nodes_per_element = element_ptr->n_nodes();

  switch (_num_nodes_per_element)
  {
    case (4):
    {
      _libmesh_element_type = ELEMENT_TET4;
      _libmesh_face_type = FACE_TRI3;
      _num_linear_nodes_per_element = 4;
      break;
    }
    case (10):
    {
      _libmesh_element_type = ELEMENT_TET10;
      _libmesh_face_type = FACE_TRI6;
      _num_linear_nodes_per_element = 4;
      break;
    }
    case (8):
    {
      _libmesh_element_type = ELEMENT_HEX8;
      _libmesh_face_type = FACE_QUAD4;
      _num_linear_nodes_per_element = 8;
      break;
    }
    case (27):
    {
      _libmesh_element_type = ELEMENT_HEX27;
      _libmesh_face_type = FACE_QUAD9;
      _num_linear_nodes_per_element = 8;
      break;
    }
    default:
    {
      mooseError("Don't know what to do with a ", _num_nodes_per_element, " node 2D element.");
      break;
    }
  }
}

void
CoupledMFEMMesh::buildLibmeshFaceInfo()
{
  switch (_libmesh_face_type)
  {
    case (FACE_EDGE2):
    {
      _num_face_nodes = 2;
      _num_face_linear_nodes = 2;
      break;
    }
    case (FACE_EDGE3):
    {
      _num_face_nodes = 3;
      _num_face_linear_nodes = 2;
      break;
    }
    case (FACE_TRI3):
    {
      _num_face_nodes = 3;
      _num_face_linear_nodes = 3;
      break;
    }
    case (FACE_TRI6):
    {
      _num_face_nodes = 6;
      _num_face_linear_nodes = 3;
      break;
    }
    case (FACE_QUAD4):
    {
      _num_face_nodes = 4;
      _num_face_linear_nodes = 4;
      break;
    }
    case (FACE_QUAD9):
    {
      _num_face_nodes = 9;
      _num_face_linear_nodes = 4;
      break;
    }
    default:
    {
      mooseError("Invalid face type (", _libmesh_face_type, ") specified.");
      break;
    }
  }
}

void
CoupledMFEMMesh::buildLibmeshElementInfo()
{
  switch (_dim)
  {
    case 2:
    {
      buildLibmesh2DElementInfo();
      break;
    }
    case 3:
    {
      buildLibmesh3DElementInfo();
      break;
    }
    default:
    {
      mooseError("Invalid dimension (", _dim, ") specified.");
      break;
    }
  }
}

void
CoupledMFEMMesh::buildLibmeshElementAndFaceInfo()
{
  buildLibmeshElementInfo();
  buildLibmeshFaceInfo();
}

void
CoupledMFEMMesh::buildMFEMMesh()
{
  // Retrieve information about the elements used within the mesh
  buildLibmeshElementAndFaceInfo();

  // Elem_ss and side_ss store information about which elements are in each sideset, and which sides
  // of those elements are contained within the sideset
  _num_sidesets = getNumSidesets();

  std::vector<std::vector<int>> elem_ss(_num_sidesets);
  std::vector<std::vector<int>> side_ss(_num_sidesets);

  // Populate the elem_ss and side_ss
  getBdrLists(elem_ss, side_ss);

  // block_ids
  std::set<subdomain_id_type> block_ids;
  getMesh().subdomain_ids(block_ids);

  int num_blocks_in_mesh = (int)(getMesh().n_subdomains());

  std::vector<size_t> num_elements_per_block(num_blocks_in_mesh);

  std::vector<int> start_of_block(num_blocks_in_mesh + 1);

  std::vector<int> ebprop(num_blocks_in_mesh);
  std::vector<int> ssprop(_num_sidesets);

  for (int i = 0; i < num_blocks_in_mesh; i++)
  {
    ebprop[i] = i + 1;
  }

  for (int i = 0; i < _num_sidesets; i++)
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
  std::vector<std::vector<int>> elem_blk(num_blocks_in_mesh);

  for (int i = 0; i < num_blocks_in_mesh; i++)
  {
    elem_blk[i] = std::vector<int>(num_elements_per_block[i] * _num_nodes_per_element);
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
      for (int el_nodes = 0; el_nodes < _num_nodes_per_element; el_nodes++)
      {
        elem_blk[iblock - 1][(elem_count * _num_nodes_per_element) + el_nodes] =
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
  std::vector<std::vector<int>> ss_node_id(_num_sidesets);

  createSidesetNodeIDs(elem_ss, side_ss, ss_node_id);

  std::vector<int> unique_vertex_ids;
  for (int iblock = 0; iblock < num_blocks_in_mesh; iblock++)
  {
    for (int jelement = 0; jelement < num_elements_per_block[iblock]; jelement++)
    {
      for (int knode = 0; knode < _num_linear_nodes_per_element; knode++)
      {
        unique_vertex_ids.push_back(1 +
                                    elem_blk[iblock][jelement * _num_nodes_per_element + knode]);
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

  std::vector<double> coordx(nNodes(), 0);
  std::vector<double> coordy(nNodes(), 0);
  std::vector<double> coordz(nNodes(), 0);

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

  const int num_elements_in_mesh = nElem();

  // Create MFEM mesh using this extremely long but necessary constructor
  _mfem_mesh = std::make_shared<MFEMMesh>(num_elements_in_mesh,
                                          coordx,
                                          coordy,
                                          coordz,
                                          cubit_to_MFEM_vertex_map,
                                          unique_vertex_ids,
                                          _libmesh_element_type,
                                          _libmesh_face_type,
                                          elem_blk,
                                          num_blocks_in_mesh,
                                          _num_nodes_per_element,
                                          num_elements_per_block,
                                          _num_linear_nodes_per_element,
                                          _num_face_nodes,
                                          _num_face_linear_nodes,
                                          _num_sidesets,
                                          _num_sides_in_ss,
                                          ss_node_id,
                                          ebprop,
                                          ssprop,
                                          3,
                                          start_of_block);
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
CoupledMFEMMesh::createSidesetNodeIDs(const std::vector<std::vector<int>> & elem_ss,
                                      const std::vector<std::vector<int>> & side_ss,
                                      std::vector<std::vector<int>> & ss_node_id)
{
  for (std::size_t i = 0; i < _num_sidesets; i++)
  {
    ss_node_id[i] = std::vector<int>(_num_sides_in_ss[i] * _num_face_nodes);

    for (std::size_t j = 0; j < _num_sides_in_ss[i]; j++)
    {
      int glob_ind = elem_ss[i][j];
      int side = side_ss[i][j];

      Elem * elem = elemPtr(glob_ind);

      std::vector<unsigned int> nodes = elem->nodes_on_side(side);

      for (int k = 0; k < _num_face_nodes; k++)
      {
        ss_node_id[i][j * _num_face_nodes + k] = elem->node_id(nodes[k]);
      }
    }
  }
}