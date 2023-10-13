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
  auto first_element_ptr = getFirstElementOnProcessor();

  _num_nodes_per_element = first_element_ptr->n_nodes();

  switch (_num_nodes_per_element)
  {
    case 3:
    {
      _libmesh_element_type = ELEMENT_TRI3;
      _libmesh_face_type = FACE_EDGE2;
      _num_corner_nodes_per_element = 3;
      break;
    }
    case 6:
    {
      _libmesh_element_type = ELEMENT_TRI6;
      _libmesh_face_type = FACE_EDGE3;
      _num_corner_nodes_per_element = 3;
      break;
    }
    case 4:
    {
      _libmesh_element_type = ELEMENT_QUAD4;
      _libmesh_face_type = FACE_EDGE2;
      _num_corner_nodes_per_element = 4;
      break;
    }
    case 9:
    {
      _libmesh_element_type = ELEMENT_QUAD9;
      _libmesh_face_type = FACE_EDGE3;
      _num_corner_nodes_per_element = 4;
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
  auto first_element_ptr = getFirstElementOnProcessor();

  _num_nodes_per_element = first_element_ptr->n_nodes();

  switch (_num_nodes_per_element)
  {
    case (4):
    {
      _libmesh_element_type = ELEMENT_TET4;
      _libmesh_face_type = FACE_TRI3;
      _num_corner_nodes_per_element = 4;
      break;
    }
    case (10):
    {
      _libmesh_element_type = ELEMENT_TET10;
      _libmesh_face_type = FACE_TRI6;
      _num_corner_nodes_per_element = 4;
      break;
    }
    case (8):
    {
      _libmesh_element_type = ELEMENT_HEX8;
      _libmesh_face_type = FACE_QUAD4;
      _num_corner_nodes_per_element = 8;
      break;
    }
    case (27):
    {
      _libmesh_element_type = ELEMENT_HEX27;
      _libmesh_face_type = FACE_QUAD9;
      _num_corner_nodes_per_element = 8;
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
      _num_face_corner_nodes = 2;
      break;
    }
    case (FACE_EDGE3):
    {
      _num_face_nodes = 3;
      _num_face_corner_nodes = 2;
      break;
    }
    case (FACE_TRI3):
    {
      _num_face_nodes = 3;
      _num_face_corner_nodes = 3;
      break;
    }
    case (FACE_TRI6):
    {
      _num_face_nodes = 6;
      _num_face_corner_nodes = 3;
      break;
    }
    case (FACE_QUAD4):
    {
      _num_face_nodes = 4;
      _num_face_corner_nodes = 4;
      break;
    }
    case (FACE_QUAD9):
    {
      _num_face_nodes = 9;
      _num_face_corner_nodes = 4;
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

void
CoupledMFEMMesh::buildLibmeshElementAndFaceInfo()
{
  buildLibmeshElementInfo();
  buildLibmeshFaceInfo();
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

      int element_id = element_ptr->id();

      std::vector<int> element_node_ids(_num_nodes_per_element);

      elements_in_block.push_back(element_id);

      for (int node_counter = 0; node_counter < _num_nodes_per_element; node_counter++)
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
      for (int knode = 0; knode < _num_corner_nodes_per_element; knode++)
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

void
CoupledMFEMMesh::buildMFEMMesh()
{
  // If the mesh is distributed and split between more than one processor,
  // we need to call allgather on each processor. This will gather the nodes
  // and elements onto each processor.
  const bool is_distributed = !getMesh().is_replicated();

  if (is_distributed && getMesh().n_processors() > 1)
  {
    getMesh().allgather();
  }

  // 1. Retrieve information about the elements used within the mesh.
  buildLibmeshElementAndFaceInfo();

  // 2. Get the unique libmesh IDs of each block in the mesh.
  std::vector<int> unique_block_ids = getLibmeshBlockIDs();

  // 3. Build maps:
  // Map from block ID --> vector of element IDs.
  // Map from element ID --> vector of global node IDs.
  std::map<int, std::vector<int>> element_ids_for_block_id;
  std::map<int, std::vector<int>> node_ids_for_element_id;

  buildElementAndNodeIDs(unique_block_ids, element_ids_for_block_id, node_ids_for_element_id);

  // 4. Create vector containing the IDs of all nodes that are on the corners of
  // elements. MFEM only requires the corner nodes.
  std::vector<int> unique_corner_node_ids;

  buildUniqueCornerNodeIDs(
      unique_corner_node_ids, unique_block_ids, element_ids_for_block_id, node_ids_for_element_id);

  // 5. We now create a map from the unique corner node ID to its index in the
  // vector. This ensures that nodes are numbered contiguously starting at index 0.
  // This will be used in the MFEMMesh initializer to renumber the global node IDs.
  // std::map<int, int> libmesh_to_mfem_corner_node_id_map;

  // for (int node_index = 0; node_index < unique_corner_node_ids.size(); node_index++)
  // {
  //   const int node_id = unique_corner_node_ids[node_index];

  //   libmesh_to_mfem_corner_node_id_map[node_id] = node_index;
  // }

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

  // TEST: print all node IDs.
  if (false)
  {
    FILE * fp = fopen("/opt/libmesh_corner_node_ids.txt", "w");
    if (!fp)
      fprintf(stderr, "Error: failed to open file.\n");

    for (int unique_node_id : unique_corner_node_ids)
    {
      auto & coordinates = coordinates_for_node_id[unique_node_id];

      fprintf(fp, "(%.2lf, %.2lf, %.2lf)\n", coordinates[0], coordinates[1], coordinates[2]);
    }

    fclose(fp);
  }

  // Test: check boundary nodes:
  if (false)
  {
    FILE * fp = fopen("/opt/libmesh_boundary_nodes.txt", "w");
    if (!fp)
      fprintf(stderr, "Error: failed to open file.\n");

    for (int boundary_id : unique_side_boundary_ids)
    {
      for (int node_id : node_ids_for_boundary_id[boundary_id])
      {
        auto & coordinates = coordinates_for_node_id[node_id];

        fprintf(fp, "(%.2lf, %.2lf, %.2lf)\n", coordinates[0], coordinates[1], coordinates[2]);
      }
    }

    fclose(fp);
  }

  if (true)
  {
    FILE * fp = fopen("/opt/libmesh_faces.txt", "w");

    for (int block_id : unique_block_ids)
    {
      for (int element_id : element_ids_for_block_id[block_id])
      {
        fprintf(fp, "Element %d:\n", element_id);

        libMesh::Elem * the_element = elemPtr(element_id);

        for (int iside = 0; iside < the_element->n_sides(); iside++)
        {
          fprintf(fp, "Face %d:\n", iside);

          auto nodes_on_side = the_element->nodes_on_side(iside);

          for (int node_id : nodes_on_side)
          {
            const int local_node_id = node_id;

            auto global_node_id = the_element->node_id(local_node_id);

            auto & coordinates = coordinates_for_node_id[global_node_id];

            fprintf(fp, "(%.2lf, %.2lf, %.2lf)\n", coordinates[0], coordinates[1], coordinates[2]);
          }
        }
      }
    }

    fclose(fp);
  }

  // Hacky test. Send all side info to MFEMMesh.
  std::map<int, std::vector<std::vector<int>>> global_face_node_ids_for_element_id;

  for (int block_id : unique_block_ids)
  {
    for (int element_id : element_ids_for_block_id[block_id])
    {
      libMesh::Elem * the_element = elemPtr(element_id);

      std::vector<std::vector<int>> global_node_ids_for_faces;

      for (int iside = 0; iside < the_element->n_sides(); iside++)
      {
        auto nodes_on_side = the_element->nodes_on_side(iside);

        std::vector<int> global_node_ids_for_face;

        for (int node_id : nodes_on_side)
        {
          const int local_node_id = node_id;

          auto global_node_id = the_element->node_id(local_node_id);

          global_node_ids_for_face.push_back(global_node_id);
        }

        global_node_ids_for_faces.push_back(global_node_ids_for_face);
      }

      global_face_node_ids_for_element_id[element_id] = global_node_ids_for_faces;
    }
  }

  // 10. Create MFEM mesh using this extremely long but necessary constructor.
  _mfem_mesh = std::make_shared<MFEMMesh>(3,
                                          nElem(),
                                          _libmesh_element_type,
                                          _libmesh_face_type,
                                          _num_face_nodes,
                                          _num_face_corner_nodes,
                                          _num_corner_nodes_per_element,
                                          unique_block_ids,
                                          unique_side_boundary_ids,
                                          unique_corner_node_ids,
                                          num_elements_for_boundary_id,
                                          global_face_node_ids_for_element_id,
                                          // libmesh_to_mfem_corner_node_id_map,
                                          element_ids_for_block_id,
                                          node_ids_for_element_id,
                                          node_ids_for_boundary_id,
                                          coordinates_for_node_id,
                                          _mfem_dof_for_libmesh_node_id);

  // Test whether the coordinates match:

  // MFEMMesh * mesh = _mfem_mesh.get();

  // int nprinted = 0;

  // for (int block_id : unique_block_ids)
  // {
  //   if (block_id == unique_block_ids[0])
  //     continue;

  //   for (int element_id : element_ids_for_block_id[block_id])
  //   {
  //     if (element_id < 100)
  //       continue;

  //     printf("************************* Element %4d: *******************************\n",
  //            element_id);

  //     for (int node_id : node_ids_for_element_id[element_id])
  //     {
  //       const int libmesh_node_id = node_id;

  //       auto & libmesh_coordinates = coordinates_for_node_id[libmesh_node_id];

  //       printf("(%2.2lf, %2.2lf, %2.2lf )\n",
  //              libmesh_coordinates[0],
  //              libmesh_coordinates[1],
  //              libmesh_coordinates[2]);
  //     }

  //     printf("\n");

  //     for (int node_id : node_ids_for_element_id[element_id])
  //     {
  //       const int libmesh_node_id = node_id;

  //       const int mfem_node_id = _mfem_dof_for_libmesh_node_id[libmesh_node_id];

  //       double mfem_coordinates[3];

  //       mesh->GetNode(mfem_node_id, mfem_coordinates);

  //       printf("(%2.2lf, %2.2lf, %2.2lf)\n",
  //              mfem_coordinates[0],
  //              mfem_coordinates[1],
  //              mfem_coordinates[2]);
  //     }

  //     if (nprinted++ > 20)
  //     {
  //       break;
  //     }
  //   }
  // }

  // for (const auto & key_value : _mfem_dof_for_libmesh_node_id)
  // {
  //   const int libmesh_node_id = key_value.first;

  //   auto & libmesh_coordinates = coordinates_for_node_id[libmesh_node_id];

  //   printf("\n\n");

  //   printf("libmesh_node %4d: { %.2lf, %.2lf, %.2lf }\n",
  //          libmesh_node_id,
  //          libmesh_coordinates[0],
  //          libmesh_coordinates[1],
  //          libmesh_coordinates[2]);

  //   const int mfem_node_id = key_value.second;

  //   double mfem_coordinates[3];

  //   mesh->GetNode(mfem_node_id, mfem_coordinates);

  //   printf("mfem_node %4d: { %.2lf, %.2lf, %.2lf }\n",
  //          mfem_node_id,
  //          mfem_coordinates[0],
  //          mfem_coordinates[1],
  //          mfem_coordinates[2]);

  //   printf("\n\n");
  // }
}

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
