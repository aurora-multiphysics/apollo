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

const Elem *
CoupledMFEMMesh::getFirstElementOnProcessor() const
{
  Elem * first_element_ptr = nullptr;

  auto local_elements_begin = getMesh().local_elements_begin();
  auto local_elements_end = getMesh().local_elements_end();

  for (auto iterator = local_elements_begin; iterator != local_elements_end; iterator++)
  {
    first_element_ptr = *iterator;
    break;
  }

  if (!first_element_ptr)
  {
    mooseError("Unable to get the first element on processor ", getMesh().processor_id());
  }

  return first_element_ptr;
}

bool
CoupledMFEMMesh::isDistributedMesh() const
{
  return (!getMesh().is_replicated() && n_processors() > 1);
}

void
CoupledMFEMMesh::buildLibmeshElementAndFaceInfo()
{
  auto first_element_ptr = getFirstElementOnProcessor();

  _element_info = CubitElementInfo(first_element_ptr->n_nodes(), _dim);
}

std::vector<int>
CoupledMFEMMesh::getLibmeshBlockIDs() const
{
  auto & libmesh = getMesh();

  // Identify all subdomains (blocks) in the entire mesh (global == true).
  std::set<subdomain_id_type> block_ids_set;
  libmesh.subdomain_ids(block_ids_set, true);

  std::vector<int> unique_block_ids(block_ids_set.size());

  int counter = 0;
  for (auto block_id : block_ids_set)
  {
    unique_block_ids[counter++] = block_id;
  }

  return unique_block_ids;
}

void
CoupledMFEMMesh::buildElementAndNodeIDs(const std::vector<int> & unique_block_ids,
                                        std::map<int, std::vector<int>> & element_ids_for_block_id,
                                        std::map<int, std::vector<int>> & node_ids_for_element_id)
{
  for (int block_id : unique_block_ids)
  {
    std::vector<int> elements_in_block;

    auto active_block_elements_begin = getMesh().active_subdomain_elements_begin(block_id);
    auto active_block_elements_end = getMesh().active_subdomain_elements_end(block_id);

    for (auto element_iterator = active_block_elements_begin;
         element_iterator != active_block_elements_end;
         element_iterator++)
    {
      auto element_ptr = *element_iterator;

      // Each block can contain a different element type but within a block, all
      // elements must be identical. Check the first element in each block.
      if (element_iterator == active_block_elements_begin &&
          element_ptr->n_nodes() != _element_info.getNumNodes())
      {
        mooseError("Multiple element types detected.\n");
        return;
      }

      const int element_id = element_ptr->id();

      std::vector<int> element_node_ids(_element_info.getNumNodes());

      elements_in_block.push_back(element_id);

      for (int node_counter = 0; node_counter < _element_info.getNumNodes(); node_counter++)
      {
        element_node_ids[node_counter] = element_ptr->node_id(node_counter);
      }

      node_ids_for_element_id[element_id] = element_node_ids;
    }

    elements_in_block.shrink_to_fit();

    // Add to map.
    element_ids_for_block_id[block_id] = elements_in_block;
  }
}

void
CoupledMFEMMesh::buildUniqueCornerNodeIDs(
    std::vector<int> & unique_corner_node_ids,
    const std::vector<int> & unique_block_ids,
    std::map<int, std::vector<int>> & element_ids_for_block_id,
    std::map<int, std::vector<int>> & node_ids_for_element_id)
{
  // Iterate through all nodes (on edge of each element) and add their global IDs
  // to the unique_corner_node_ids vector.
  for (int block_id : unique_block_ids)
  {
    auto & element_ids = element_ids_for_block_id[block_id];

    for (int element_id : element_ids)
    {
      auto & node_ids = node_ids_for_element_id[element_id];

      // Only use the nodes on the edge of the element!
      for (int knode = 0; knode < _element_info.getNumCornerNodes(); knode++)
      {
        unique_corner_node_ids.push_back(node_ids[knode]);
      }
    }
  }

  // Sort unique_vertex_ids in ascending order and remove duplicate node IDs.
  std::sort(unique_corner_node_ids.begin(), unique_corner_node_ids.end());

  auto new_end = std::unique(unique_corner_node_ids.begin(), unique_corner_node_ids.end());

  unique_corner_node_ids.resize(std::distance(unique_corner_node_ids.begin(), new_end));
}

bool
CoupledMFEMMesh::buildHex27CenterOfFaceNodeIDsForElement(
    const int element_id, std::array<int, 6> & center_of_face_node_ids)
{
  if (_element_info.getElementType() != CubitElementInfo::ELEMENT_HEX27)
  {
    return false;
  }

  libMesh::Elem * the_element = elemPtr(element_id);
  if (!the_element || the_element->n_sides() != 6)
  {
    return false;
  }

  for (int iface = 0; iface < the_element->n_sides(); iface++)
  {
    // Last node id corresponds to center of face node.
    auto local_node_ids_for_face = the_element->nodes_on_side(iface);
    const int last_local_node_id = local_node_ids_for_face.back();

    // Local --> global node id.
    center_of_face_node_ids[iface] = the_element->node_id(last_local_node_id);
  }

  return true;
}

bool
CoupledMFEMMesh::buildHex27CenterOfFaceNodeIDs(
    std::map<int, std::vector<int>> & element_ids_for_block_id,
    std::map<int, std::array<int, 6>> & center_of_face_node_ids_for_hex27_element_id)
{
  if (_element_info.getElementType() != CubitElementInfo::ELEMENT_HEX27)
  {
    return false;
  }

  center_of_face_node_ids_for_hex27_element_id.clear();

  std::array<int, 6> center_of_face_node_ids;

  for (const auto & key_value : element_ids_for_block_id)
  {
    auto & element_ids_for_block = key_value.second;

    for (int element_id : element_ids_for_block)
    {
      if (buildHex27CenterOfFaceNodeIDsForElement(element_id, center_of_face_node_ids) != true)
      {
        return false;
      }

      center_of_face_node_ids_for_hex27_element_id[element_id] = center_of_face_node_ids;
    }
  }

  return true;
}

void
CoupledMFEMMesh::buildMFEMMesh()
{
  // 1. If the mesh is distributed and split between more than one processor,
  // we need to call allgather on each processor. This will gather the nodes
  // and elements onto each processor.
  if (isDistributedMesh())
  {
    getMesh().allgather();
  }

  // 2. Retrieve information about the elements used within the mesh.
  buildLibmeshElementAndFaceInfo();

  // 3. Get the unique libmesh IDs of each block in the mesh.
  std::vector<int> unique_block_ids = getLibmeshBlockIDs();

  // 4. Build maps:
  // Map from block ID --> vector of element IDs.
  // Map from element ID --> vector of global node IDs.
  std::map<int, std::vector<int>> element_ids_for_block_id;
  std::map<int, std::vector<int>> node_ids_for_element_id;

  buildElementAndNodeIDs(unique_block_ids, element_ids_for_block_id, node_ids_for_element_id);

  // 5. Create vector containing the IDs of all nodes that are on the corners of
  // elements. MFEM only requires the corner nodes.
  std::vector<int> unique_corner_node_ids;

  buildUniqueCornerNodeIDs(
      unique_corner_node_ids, unique_block_ids, element_ids_for_block_id, node_ids_for_element_id);

  // 6. Create a map to hold the x, y, z coordinates for each unique node.
  std::map<int, std::array<double, 3>> coordinates_for_node_id;

  for (auto node_ptr : getMesh().node_ptr_range())
  {
    auto & node = *node_ptr;

    std::array<double, 3> coordinates = {node(0), node(1), node(2)};

    coordinates_for_node_id[node.id()] = coordinates;
  }

  // 7.
  // element_ids_for_boundary_id stores the ids of each element on each boundary.
  // side_ids_for_boundary_id stores the sides of those elements that are on each boundary.
  // num_elements_for_boundary_id stores the number of elements contained on each boundary.
  std::map<int, std::vector<int>> element_ids_for_boundary_id;
  std::map<int, std::vector<int>> side_ids_for_boundary_id;
  std::map<int, int> num_elements_for_boundary_id;

  buildBoundaryInfo(
      element_ids_for_boundary_id, side_ids_for_boundary_id, num_elements_for_boundary_id);

  // 8. Get a vector containing all boundary IDs on sides of semi-local elements.
  std::vector<int> unique_side_boundary_ids = getSideBoundaryIDs();

  // 9.
  // node_ids_for_boundary_id maps from the boundary ID to a vector containing
  // the nodes of each element on the boundary that correspond to the face of the boundary.
  std::map<int, std::vector<int>> node_ids_for_boundary_id;

  buildBoundaryNodeIDs(unique_side_boundary_ids,
                       element_ids_for_boundary_id,
                       side_ids_for_boundary_id,
                       node_ids_for_boundary_id);

  // 10.
  // Call the correct initializer.
  const int element_order = _element_info.getOrder();
  if (element_order == 1)
  {
    _mfem_mesh = std::make_shared<MFEMMesh>(nElem(),
                                            _element_info,
                                            unique_block_ids,
                                            unique_side_boundary_ids,
                                            unique_corner_node_ids,
                                            num_elements_for_boundary_id,
                                            element_ids_for_block_id,
                                            node_ids_for_element_id,
                                            node_ids_for_boundary_id,
                                            coordinates_for_node_id);
  }
  else if (element_order == 2)
  {
    std::map<int, std::array<int, 6>> center_of_face_node_ids_for_hex27_element_ids;

    if (_element_info.getElementType() == CubitElementInfo::ELEMENT_HEX27)
    {
      buildHex27CenterOfFaceNodeIDs(element_ids_for_block_id,
                                    center_of_face_node_ids_for_hex27_element_ids);
    }

    _mfem_mesh = std::make_shared<MFEMMesh>(nElem(),
                                            _element_info,
                                            unique_block_ids,
                                            unique_side_boundary_ids,
                                            unique_corner_node_ids,
                                            num_elements_for_boundary_id,
                                            element_ids_for_block_id,
                                            node_ids_for_element_id,
                                            node_ids_for_boundary_id,
                                            coordinates_for_node_id,
                                            _second_order_node_bimap,
                                            center_of_face_node_ids_for_hex27_element_ids);
  }
  else
  {
    mooseError("Unsupported element type of order ", element_order, ".");
  }
}

std::unique_ptr<int[]>
CoupledMFEMMesh::getMeshPartitioning()
{
  // Return NULL if mesh is not distributed.
  if (!isDistributedMesh())
  {
    return nullptr;
  }

  // Call allgather because we need all element information on each processor.
  getMesh().allgather();

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
  // There are two cases:
  // 1. construct MFEMParMesh from distributed MOOSE mesh -- partitioning array non-null.
  // 2. construct MFEMParMesh from serial      MOOSE mesh -- partitioning array null.
  auto partitioning = getMeshPartitioning();

  int * partitioning_raw_ptr = partitioning ? partitioning.get() : nullptr;

  _mfem_par_mesh =
      std::make_shared<MFEMParMesh>(MPI_COMM_WORLD, getMFEMMesh(), partitioning_raw_ptr);
  _mfem_mesh.reset(); // Lower reference count of serial mesh since no longer needed.
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
    std::vector<int> boundary_nodes(boundary_element_ids.size() * _element_info.getNumFaceNodes());

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
      for (int knode = 0; knode < _element_info.getNumFaceNodes(); knode++)
      {
        // Get the global node ID of each node.
        const int local_node_id = nodes_of_element_on_side[knode];
        const int global_node_id = element_ptr->node_id(local_node_id);

        // Add to boundary_nodes vector.
        boundary_nodes[jelement * _element_info.getNumFaceNodes() + knode] = global_node_id;
      }
    }

    // Add to node_ids_for_boundary_id for boundary_id.
    node_ids_for_boundary_id[boundary_id] = boundary_nodes;
  }
}
