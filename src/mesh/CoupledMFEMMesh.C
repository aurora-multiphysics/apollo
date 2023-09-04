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

void
CoupledMFEMMesh::buildBoundaryInfo(std::map<int, std::vector<int>> & element_ids_for_boundary_id,
                                   std::map<int, std::vector<int>> & side_ids_for_boundary_id,
                                   std::map<int, int> & num_elements_for_boundary_id)
{
  buildBndElemList();

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

  // Sort.
  std::sort(unique_boundary_ids.begin(), unique_boundary_ids.end());

  // Run through the (key, value) pairs in the boundary_ids_map map.
  for (const auto & key_value_pair : boundary_ids_map)
  {
    auto boundary_id = key_value_pair.first;

    auto element_ids = key_value_pair.second.element_ids;
    auto side_ids = key_value_pair.second.side_ids;

    element_ids_for_boundary_id[boundary_id] = element_ids;
    side_ids_for_boundary_id[boundary_id] = side_ids;

    num_elements_for_boundary_id[boundary_id] = element_ids.size();
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

std::vector<int>
CoupledMFEMMesh::getSideBoundaryIDs() const
{
  const libMesh::BoundaryInfo & boundary_info = getMesh().get_boundary_info();
  const std::set<boundary_id_type> & side_boundary_ids_set = boundary_info.get_side_boundary_ids();

  std::vector<int> side_boundary_ids(side_boundary_ids_set.size());

  int counter = 0;
  for (auto side_boundary_id : side_boundary_ids_set)
  {
    side_boundary_ids[counter++] = side_boundary_id;
  }

  std::sort(side_boundary_ids.begin(), side_boundary_ids.end());

  return side_boundary_ids;
}

void
CoupledMFEMMesh::buildLibmeshElementAndFaceInfo()
{
  buildLibmeshElementInfo();
  buildLibmeshFaceInfo();
}

/**
 * Blocks/subdomains are separate subsets of the mesh that could have different
 * material properties etc. This method returns a vector containing the unique
 * IDs of each block in the mesh. This will be passed to the MFEMMesh constructor
 * which sets the attribute of each element to the ID of the block that it is a
 * part of.
 */
std::vector<int>
CoupledMFEMMesh::getLibmeshBlockIDs() const
{
  auto & libmesh = getMesh();

  // TODO: - may not work with distributed meshes.
  std::set<subdomain_id_type> block_ids_set;
  libmesh.subdomain_ids(block_ids_set);

  std::vector<int> unique_block_ids(block_ids_set.size());

  int counter = 0;
  for (auto block_id : block_ids_set)
  {
    unique_block_ids[counter++] = block_id;
  }

  return unique_block_ids;
}

void
CoupledMFEMMesh::buildMFEMMesh()
{
  // Retrieve information about the elements used within the mesh
  buildLibmeshElementAndFaceInfo();

  // Get a vector containing all boundary IDs on sides of semi-local elements.
  std::vector<int> unique_side_boundary_ids = getSideBoundaryIDs();

  // element_ids_for_boundary_id stores the ids of each element on each boundary.
  // side_ids_for_boundary_id stores the sides of those elements that are on each boundary.
  // num_elements_for_boundary_id stores the number of elements contained on each boundary.
  std::map<int, std::vector<int>> element_ids_for_boundary_id;
  std::map<int, std::vector<int>> side_ids_for_boundary_id;
  std::map<int, int> num_elements_for_boundary_id;

  buildBoundaryInfo(
      element_ids_for_boundary_id, side_ids_for_boundary_id, num_elements_for_boundary_id);

  // Get the unique libmesh IDs of each block in the mesh. The block IDs are
  // 1-based and are numbered continuously.
  std::vector<int> unique_block_ids = getLibmeshBlockIDs();

  // num_elements_per_block maps from the block_id to an unsigned integer
  // containing the number of elements present in the block.
  std::map<int, size_t> num_elements_per_block;

  // Loops to set num_elements_per_block.
  for (int block_id : unique_block_ids)
  {
    int num_elements_in_block_counter = 0;

    for (libMesh::MeshBase::element_iterator element_ptr =
             getMesh().active_subdomain_elements_begin(block_id);
         element_ptr != getMesh().active_subdomain_elements_end(block_id);
         element_ptr++)
    {
      num_elements_in_block_counter++;
    }

    num_elements_per_block[block_id] = num_elements_in_block_counter;
  }

  // element_nodes_for_block_id maps from the block_id to a vector containing the nodes of all
  // elements in the block.
  std::map<int, std::vector<int>> element_nodes_for_block_id;

  std::map<int, std::vector<int>> element_ids_for_block_id;
  std::map<int, std::vector<int>> node_ids_for_element_id;

  std::map<int, int> block_id_for_element_id; // Reverse map from element id --> block_id.

  for (int block_id : unique_block_ids)
  {
    // Create vector to hold nodes of all elements in current block.
    const int num_nodes_in_block = num_elements_per_block[block_id] * _num_nodes_per_element;

    std::vector<int> element_nodes_in_block(num_nodes_in_block);

    std::vector<int> elements_in_block(num_elements_per_block[block_id]);

    int element_counter = 0;

    for (auto element_iterator = getMesh().active_subdomain_elements_begin(block_id);
         element_iterator != getMesh().active_subdomain_elements_end(block_id);
         element_iterator++)
    {
      auto element_ptr = *element_iterator;

      int element_id = element_ptr->id();

      std::vector<int> element_node_ids(_num_nodes_per_element);

      block_id_for_element_id[element_id] = block_id;

      elements_in_block[element_counter] = element_id;

      const int node_offset = element_counter * _num_nodes_per_element;

      for (int node_counter = 0; node_counter < _num_nodes_per_element; node_counter++)
      {
        element_nodes_in_block[node_offset + node_counter] = element_ptr->node_id(node_counter);

        element_node_ids[node_counter] = element_ptr->node_id(node_counter);
      }

      node_ids_for_element_id[element_id] = element_node_ids;

      element_counter++;
    }

    // Add to map.
    element_nodes_for_block_id[block_id] = element_nodes_in_block;
    element_ids_for_block_id[block_id] = elements_in_block;
  }

  // node_ids_for_boundary_id maps from the boundary_id to a vector containing nodes of each
  // element on the boundary that correspond to the face of the boundary.
  std::map<int, std::vector<int>> node_ids_for_boundary_id;

  buildBoundaryNodeIDs(unique_side_boundary_ids,
                       element_ids_for_boundary_id,
                       side_ids_for_boundary_id,
                       node_ids_for_boundary_id);

  std::vector<int> unique_vertex_ids;
  for (int block_id : unique_block_ids)
  {
    auto & element_ids = element_ids_for_block_id[block_id];

    for (int element_id : element_ids)
    {
      auto & node_ids = node_ids_for_element_id[element_id];

      // Only use the nodes on the edge of the element!
      for (int knode = 0; knode < _num_linear_nodes_per_element; knode++)
      {
        unique_vertex_ids.push_back(1 + node_ids[knode]);
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
                                          element_ids_for_block_id,
                                          node_ids_for_element_id,
                                          block_id_for_element_id,
                                          element_nodes_for_block_id,
                                          _num_linear_nodes_per_element,
                                          _num_face_nodes,
                                          _num_face_linear_nodes,
                                          num_elements_for_boundary_id,
                                          node_ids_for_boundary_id,
                                          unique_block_ids,
                                          unique_side_boundary_ids,
                                          3);
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
CoupledMFEMMesh::buildBoundaryNodeIDs(const std::vector<int> & unique_side_boundary_ids,
                                      std::map<int, std::vector<int>> & element_ids_for_boundary_id,
                                      std::map<int, std::vector<int>> & side_ids_for_boundary_id,
                                      std::map<int, std::vector<int>> & node_ids_for_boundary_id)
{
  // Iterate over all boundary IDs.
  for (int boundary_id : unique_side_boundary_ids)
  {
    // Get element IDs  of element on boundary (and their sides that are on boundary).
    auto & boundary_element_ids = element_ids_for_boundary_id[boundary_id];
    auto & boundary_element_sides = side_ids_for_boundary_id[boundary_id];

    // Create vector to hold all nodes on boundary.
    std::vector<int> boundary_nodes(boundary_element_ids.size() * _num_face_nodes);

    // Iterate over elements on boundary.
    for (int jelement = 0; jelement < boundary_element_ids.size(); jelement++)
    {
      // Get element ID and the boundary side.
      const int boundary_element_global_id = boundary_element_ids[jelement];
      const int boundary_element_side = boundary_element_sides[jelement];

      Elem * element_ptr = elemPtr(boundary_element_global_id);

      // Get vector of local node IDs on boundary side of element.
      auto nodes_of_element_on_side = element_ptr->nodes_on_side(boundary_element_side);

      // Iterate over nodes on boundary side of element.
      for (int knode = 0; knode < _num_face_nodes; knode++)
      {
        // Get the global node ID of each node.
        const int local_node_id = nodes_of_element_on_side[knode];
        const int global_node_id = element_ptr->node_id(local_node_id);

        // Add to boundary_nodes vector.
        boundary_nodes[jelement * _num_face_nodes + knode] = global_node_id;
      }
    }

    // Add to node_ids_for_boundary_id for boundary_id.
    node_ids_for_boundary_id[boundary_id] = boundary_nodes;
  }
}