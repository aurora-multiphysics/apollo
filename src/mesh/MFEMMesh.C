#pragma once
#include "MFEMMesh.h"
#include "MooseError.h"

MFEMMesh::MFEMMesh(const int num_dimensions,
                   const int num_elements_in_mesh,
                   const int libmesh_element_type,
                   const int libmesh_face_type,
                   const int num_face_nodes,
                   const int num_face_corner_nodes,
                   const int num_corner_nodes_per_element,
                   const std::vector<int> & unique_block_ids,
                   const std::vector<int> & unique_side_boundary_ids,
                   const std::vector<int> & unique_corner_node_ids,
                   std::map<int, int> & num_elements_for_boundary_id,
                   std::map<int, int> & libmesh_to_mfem_corner_node_id_map,
                   std::map<int, std::vector<int>> & element_ids_for_block_id,
                   std::map<int, std::vector<int>> & node_ids_for_element_id,
                   std::map<int, std::vector<int>> & node_ids_for_boundary_id,
                   std::map<int, std::array<double, 3>> & coordinates_for_unique_corner_node_id)
{
  // Set dimensions.
  Dim = num_dimensions;
  spaceDim = Dim; // Is this always the case?

  // Create the vertices.
  buildMFEMVertices(unique_corner_node_ids, coordinates_for_unique_corner_node_id, num_dimensions);

  // Create the mesh elements.
  buildMFEMElements(num_elements_in_mesh,
                    libmesh_element_type,
                    num_corner_nodes_per_element,
                    unique_block_ids,
                    element_ids_for_block_id,
                    node_ids_for_element_id,
                    libmesh_to_mfem_corner_node_id_map);

  // Create the boundary elements.
  buildMFEMBoundaryElements(libmesh_face_type,
                            num_face_nodes,
                            num_face_corner_nodes,
                            unique_side_boundary_ids,
                            num_elements_for_boundary_id,
                            node_ids_for_boundary_id,
                            libmesh_to_mfem_corner_node_id_map);

  // Handle higher-order meshes.
  const int order = getOrderFromLibmeshElementType(libmesh_element_type);

  if (order == 2)
  {
    handleQuadraticFESpace(libmesh_element_type,
                           unique_block_ids,
                           element_ids_for_block_id,
                           node_ids_for_element_id,
                           coordinates_for_unique_corner_node_id);
  }

  // Finalize mesh method is needed to fully finish constructing the mesh.
  FinalizeMesh();
}

MFEMMesh::MFEMMesh(std::string mesh_fname, int generate_edges, int refine, bool fix_orientation)
{
  SetEmpty();

  mfem::named_ifgzstream mesh_fstream(mesh_fname);
  if (!mesh_fstream) // TODO: - can this be NULL?
  {
    mooseError("Failed to read '" + mesh_fname + "'\n");
  }
  else
  {
    Load(mesh_fstream, generate_edges, refine, fix_orientation);
  }
}

void
MFEMMesh::buildMFEMVertices(
    const std::vector<int> & unique_corner_node_ids,
    std::map<int, std::array<double, 3>> & coordinates_for_unique_corner_node_id,
    const int num_dimensions)
{
  NumOfVertices = unique_corner_node_ids.size();
  vertices.SetSize(NumOfVertices);

  // Iterate over the global IDs of each unqiue corner node from the MOOSE mesh.
  const bool use_z_component = (num_dimensions == 3);

  int ivertex = 0;

  for (int global_node_id : unique_corner_node_ids)
  {
    // Get the xyz coordinates associated with the node.
    auto & coordinates = coordinates_for_unique_corner_node_id[global_node_id];

    // Set xyz components.
    vertices[ivertex](0) = coordinates[0];
    vertices[ivertex](1) = coordinates[1];

    if (use_z_component)
    {
      vertices[ivertex](2) = coordinates[2];
    }

    ivertex++;
  }
}

void
MFEMMesh::buildMFEMElements(const int num_elements_in_mesh,
                            const int libmesh_element_type,
                            const int num_corner_nodes_per_element,
                            const std::vector<int> & unique_block_ids,
                            std::map<int, std::vector<int>> & element_ids_for_block_id,
                            std::map<int, std::vector<int>> & node_ids_for_element_id,
                            std::map<int, int> & libmesh_to_mfem_corner_node_id_map)
{
  // Set mesh elements.
  NumOfElements = num_elements_in_mesh;
  elements.SetSize(num_elements_in_mesh);

  int renumbered_vertex_ids[num_corner_nodes_per_element];

  int ielement = 0;

  for (int block_id : unique_block_ids) // Iterate over blocks.
  {
    auto & element_ids = element_ids_for_block_id[block_id];

    for (int element_id : element_ids) // Iterate over elements in block.
    {
      auto & node_ids = node_ids_for_element_id[element_id];

      // Iterate over ONLY the corner nodes in the element.
      for (int inode = 0; inode < num_corner_nodes_per_element; inode++)
      {
        const int global_node_id = node_ids[inode];

        // Map from the global node ID --> index in the unique_corner_node_ids vector.
        renumbered_vertex_ids[inode] = libmesh_to_mfem_corner_node_id_map[global_node_id];
      }

      elements[ielement++] =
          buildMFEMElement(libmesh_element_type, renumbered_vertex_ids, block_id);
    }
  }
}

void
MFEMMesh::buildMFEMBoundaryElements(const int libmesh_face_type,
                                    const int num_face_nodes,
                                    const int num_face_corner_nodes,
                                    const std::vector<int> & unique_side_boundary_ids,
                                    std::map<int, int> & num_elements_for_boundary_id,
                                    std::map<int, std::vector<int>> & node_ids_for_boundary_id,
                                    std::map<int, int> & libmesh_to_mfem_corner_node_id_map)
{
  // Set boundary elements:
  NumOfBdrElements = 0;

  for (int boundary_id : unique_side_boundary_ids)
  {
    NumOfBdrElements += num_elements_for_boundary_id[boundary_id];
  }

  boundary.SetSize(NumOfBdrElements);

  int iboundary = 0;

  int renumbered_vertex_ids[num_face_corner_nodes];

  for (int boundary_id : unique_side_boundary_ids)
  {
    auto boundary_nodes = node_ids_for_boundary_id[boundary_id];

    for (int jelement = 0; jelement < num_elements_for_boundary_id[boundary_id]; jelement++)
    {
      for (int knode = 0; knode < num_face_corner_nodes; knode++)
      {
        const int node_global_index = boundary_nodes[jelement * num_face_nodes + knode];

        // Renumber vertex ("node") IDs so they're contiguous and start from 0.
        renumbered_vertex_ids[knode] = libmesh_to_mfem_corner_node_id_map[node_global_index];
      }

      boundary[iboundary++] =
          buildMFEMFaceElement(libmesh_face_type, renumbered_vertex_ids, boundary_id);
    }
  }
}

mfem::Element *
MFEMMesh::buildMFEMElement(const int element_type, const int * vertex_ids, const int block_id)
{
  mfem::Element * new_element = nullptr;

  switch (element_type)
  {
    case ELEMENT_TRI3:
    case ELEMENT_TRI6:
    {
      new_element = new mfem::Triangle(vertex_ids, block_id);
      break;
    }
    case ELEMENT_QUAD4:
    case ELEMENT_QUAD9:
    {
      new_element = new mfem::Quadrilateral(vertex_ids, block_id);
      break;
    }
    case ELEMENT_TET4:
    case ELEMENT_TET10:
    {
#ifdef MFEM_USE_MEMALLOC
      new_element = TetMemory.Alloc();
      new_element->SetVertices(vertex_ids);
      new_element->SetAttribute(block_id);
#else
      new_element = new mfem::Tetrahedron(vertex_ids, block_id);
#endif
      break;
    }
    case ELEMENT_HEX8:
    case ELEMENT_HEX27:
    {
      new_element = new mfem::Hexahedron(vertex_ids, block_id);
      break;
    }
    default:
    {
      mooseError("Unsupported element type specified.\n");
      break;
    }
  }

  return new_element;
}

mfem::Element *
MFEMMesh::buildMFEMFaceElement(const int face_type, const int * vertex_ids, const int boundary_id)
{
  mfem::Element * new_face = nullptr;

  switch (face_type)
  {
    case FACE_EDGE2:
    case FACE_EDGE3:
    {
      new_face = new mfem::Segment(vertex_ids, boundary_id);
      break;
    }
    case FACE_TRI3:
    case FACE_TRI6:
    {
      new_face = new mfem::Triangle(vertex_ids, boundary_id);
      break;
    }
    case FACE_QUAD4:
    case FACE_QUAD9:
    {
      new_face = new mfem::Quadrilateral(vertex_ids, boundary_id);
      break;
    }
    default:
    {
      mooseError("Unsupported face type encountered.\n");
      break;
    }
  }

  return new_face;
}

void
MFEMMesh::handleQuadraticFESpace(
    const int libmesh_element_type,
    const std::vector<int> & unique_block_ids,
    std::map<int, std::vector<int>> & element_ids_for_block_id,
    std::map<int, std::vector<int>> & node_ids_for_element_id,
    std::map<int, std::array<double, 3>> & coordinates_for_unique_corner_node_id)
{
  const int mfem_to_libmesh_tet10[] = {1, 2, 3, 4, 5, 7, 8, 6, 9, 10};

  const int mfem_to_libmesh_hex27[] = {1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 17, 18,
                                       19, 20, 13, 14, 15, 16, 22, 26, 25, 27, 24, 23, 21};

  const int mfem_to_libmesh_tri6[] = {1, 2, 3, 4, 5, 6};

  const int mfem_to_libmesh_quad9[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

  int * mfem_to_libmesh_map = nullptr;

  switch (libmesh_element_type)
  {
    case ELEMENT_TRI6:
      mfem_to_libmesh_map = (int *)mfem_to_libmesh_tri6;
      break;
    case ELEMENT_QUAD9:
      mfem_to_libmesh_map = (int *)mfem_to_libmesh_quad9;
      break;
    case ELEMENT_TET10:
      mfem_to_libmesh_map = (int *)mfem_to_libmesh_tet10;
      break;
    case ELEMENT_HEX27:
      mfem_to_libmesh_map = (int *)mfem_to_libmesh_hex27;
      break;
    case ELEMENT_TRI3:
    case ELEMENT_QUAD4:
    case ELEMENT_TET4:
    case ELEMENT_HEX8:
    default:
      mooseError("Linear elements detected when order is 2.");
      break;
  }

  FinalizeTopology();

  // Define quadratic FE space.
  mfem::FiniteElementCollection * finite_element_collection = new mfem::H1_FECollection(2, 3);
  mfem::FiniteElementSpace * finite_element_space =
      new mfem::FiniteElementSpace(this, finite_element_collection, Dim, mfem::Ordering::byVDIM);

  Nodes = new mfem::GridFunction(finite_element_space);
  Nodes->MakeOwner(finite_element_collection); // Nodes will destroy 'finite_element_collection'
                                               // and 'finite_element_space'

  int ielement = 0;

  // Loop over blocks.
  for (int block_id : unique_block_ids)
  {
    // Get vector containing all element IDs in block.
    auto & element_ids = element_ids_for_block_id[block_id];

    // Loop over element IDs.
    for (int element_id : element_ids)
    {
      // Get vector containing ALL node global IDs for element.
      auto & node_ids = node_ids_for_element_id[element_id];

      // Sets DOF array for element. Higher-order (second-order) elements contain
      // additional nodes between corner nodes.
      mfem::Array<int> dofs;
      finite_element_space->GetElementDofs(ielement, dofs);

      // Deep copy of DOFs array.
      mfem::Array<int> vdofs = dofs;
      vdofs.SetSize(dofs.Size());

      finite_element_space->DofsToVDofs(vdofs);

      // Iterate over dofs array.
      for (int j = 0; j < dofs.Size(); j++)
      {
        // NB: the map is 1-based to we need to subtract 1.
        const int libmesh_node_index = mfem_to_libmesh_map[j] - 1;

        const int global_node_id = node_ids[libmesh_node_index];

        // Extract node's coordinates:
        auto coordinates = coordinates_for_unique_corner_node_id[global_node_id];

        (*Nodes)(vdofs[j]) = coordinates[0];
        (*Nodes)(vdofs[j] + 1) = coordinates[1];

        if (Dim == 3)
        {
          (*Nodes)(vdofs[j] + 2) = coordinates[2];
        }
      }

      ielement++;
    }
  }
}

const int
MFEMMesh::getOrderFromLibmeshElementType(int libmesh_element_type) const
{
  int order;

  switch (libmesh_element_type)
  {
    case ELEMENT_TRI3:
    case ELEMENT_QUAD4:
    case ELEMENT_TET4:
    case ELEMENT_HEX8:
      order = 1;
      break;
    case ELEMENT_TRI6:
    case ELEMENT_QUAD9:
    case ELEMENT_TET10:
    case ELEMENT_HEX27:
      order = 2;
      break;
    default:
      order = 0;
      break;
  }

  return order;
}
