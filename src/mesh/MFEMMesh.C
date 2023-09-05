#pragma once
#include "MFEMMesh.h"
#include "MooseError.h"

/**
 * BuildMFEMVertices
 *
 * This method is called to construct the vertices of the MFEM mesh. Note that
 * the vertices (named "nodes" in MOOSE) are ONLY at the corners of elements.
 * These are referred to as "linear nodes" in MOOSE.
 */
void
MFEMMesh::BuildMFEMVertices(
    std::vector<int> & unique_linear_node_ids,
    std::map<int, std::array<double, 3>> & coordinates_for_unique_linear_node_id,
    const int num_dimensions)
{
  NumOfVertices = unique_linear_node_ids.size();
  vertices.SetSize(NumOfVertices);

  // Iterate over the global IDs of each unqiue linear node from the MOOSE mesh.
  const bool use_z_component = (num_dimensions == 3);

  int ivertex = 0;

  for (int global_node_id : unique_linear_node_ids)
  {
    // Get the xyz coordinates associated with the node.
    auto & coordinates = coordinates_for_unique_linear_node_id[global_node_id];

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

/**
 * BuildMFEMElement
 *
 * This method is called to construct an element of the MFEMMesh.
 */
mfem::Element *
MFEMMesh::BuildMFEMElement(const int element_type, const int * vertex_ids, const int block_id)
{
  mfem::Element * new_element = nullptr;

  switch (element_type) // TODO: - we need a default case.
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

/**
 * BuildMFEMElements
 *
 * This method is called to construct the elements of the MFEMMesh.
 */
void
MFEMMesh::BuildMFEMElements(const int num_elements_in_mesh,
                            const int libmesh_element_type,
                            const int num_linear_nodes_per_element,
                            const std::vector<int> & unique_block_ids,
                            std::map<int, std::vector<int>> & element_ids_for_block_id,
                            std::map<int, std::vector<int>> & node_ids_for_element_id,
                            std::map<int, int> & index_for_unique_linear_node_id)
{
  // Set mesh elements.
  NumOfElements = num_elements_in_mesh;
  elements.SetSize(num_elements_in_mesh);

  int renumbered_vertex_ids[8]; // TODO: - replace magic number.

  int ielement = 0;

  for (int block_id : unique_block_ids) // Iterate over blocks.
  {
    auto & element_ids = element_ids_for_block_id[block_id];

    for (int element_id : element_ids) // Iterate over elements in block.
    {
      auto & node_ids = node_ids_for_element_id[element_id];

      // Iterate over ONLY the linear nodes in the element.
      for (int inode = 0; inode < num_linear_nodes_per_element; inode++)
      {
        int global_node_id = node_ids[inode];

        // Map from the global node ID --> index in the unique_linear_node_ids vector.
        renumbered_vertex_ids[inode] = index_for_unique_linear_node_id[global_node_id];
      }

      elements[ielement++] =
          BuildMFEMElement(libmesh_element_type, renumbered_vertex_ids, block_id);
    }
  }
}

// Constructor to create an MFEM mesh from VTK data structures. These data
// structures are obtained by the methods found in MFEMproblem
MFEMMesh::MFEMMesh(int num_elements_in_mesh,
                   std::map<int, std::array<double, 3>> & coordinates_for_unique_linear_node_id,
                   std::map<int, int> & index_for_unique_linear_node_id,
                   std::vector<int> & unique_linear_node_ids,
                   int libmesh_element_type,
                   int libmesh_face_type,
                   std::map<int, std::vector<int>> element_ids_for_block_id,
                   std::map<int, std::vector<int>> node_ids_for_element_id,
                   int num_linear_nodes_per_element,
                   int num_face_nodes,
                   int num_face_linear_nodes,
                   std::map<int, int> & num_elements_for_boundary_id,
                   std::map<int, std::vector<int>> & node_ids_for_boundary_id,
                   const std::vector<int> & unique_block_ids,
                   const std::vector<int> & unique_side_boundary_ids,
                   int num_dimensions)
{

  const int mfemToLibmeshTet10[] = {1, 2, 3, 4, 5, 7, 8, 6, 9, 10};

  const int mfemToLibmeshHex27[] = {1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 17, 18,
                                    19, 20, 13, 14, 15, 16, 21, 22, 23, 24, 25, 26, 27};

  const int mfemToLibmeshTri6[] = {1, 2, 3, 4, 5, 6};
  const int mfemToLibmeshQuad9[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

  const int order = getOrderFromLibmeshElementType(libmesh_element_type);

  // Set dimensions.
  Dim = num_dimensions;
  spaceDim = Dim;

  // Create the vertices.
  BuildMFEMVertices(unique_linear_node_ids, coordinates_for_unique_linear_node_id, num_dimensions);

  // Create the mesh elements.
  BuildMFEMElements(num_elements_in_mesh,
                    libmesh_element_type,
                    num_linear_nodes_per_element,
                    unique_block_ids,
                    element_ids_for_block_id,
                    node_ids_for_element_id,
                    index_for_unique_linear_node_id);

  // Set boundary elements:
  NumOfBdrElements = 0;

  for (int boundary_id : unique_side_boundary_ids)
  {
    NumOfBdrElements += num_elements_for_boundary_id[boundary_id];
  }

  boundary.SetSize(NumOfBdrElements);

  int iboundary = 0;

  int renumbered_vertex_ids[8];

  // TODO: - rewrite this in the same way that we wrote element_ids_for_boundary_ids etc...
  for (int boundary_id : unique_side_boundary_ids)
  {
    auto boundary_nodes = node_ids_for_boundary_id[boundary_id];

    for (int jelement = 0; jelement < num_elements_for_boundary_id[boundary_id]; jelement++)
    {
      for (int knode = 0; knode < num_face_linear_nodes; knode++)
      {
        const int node_global_index = boundary_nodes[jelement * num_face_nodes + knode];

        renumbered_vertex_ids[knode] = index_for_unique_linear_node_id[node_global_index];
      }

      switch (libmesh_face_type)
      {
        case FACE_EDGE2:
        case FACE_EDGE3:
        {
          boundary[iboundary] = new mfem::Segment(renumbered_vertex_ids, boundary_id);
          break;
        }
        case FACE_TRI3:
        case FACE_TRI6:
        {
          boundary[iboundary] = new mfem::Triangle(renumbered_vertex_ids, boundary_id);
          break;
        }
        case FACE_QUAD4:
        case FACE_QUAD9:
        {
          boundary[iboundary] = new mfem::Quadrilateral(renumbered_vertex_ids, boundary_id);
          break;
        }
        default:
        {
          mooseError("Unsupported face type encountered.\n");
          break;
        }
      }

      iboundary++;
    }
  }

  if (order == 2)
  {
    int * mfemToLibmeshMap = nullptr;

    switch (libmesh_element_type)
    {
      case ELEMENT_TRI6:
        mfemToLibmeshMap = (int *)mfemToLibmeshTri6;
        break;
      case ELEMENT_QUAD9:
        mfemToLibmeshMap = (int *)mfemToLibmeshQuad9;
        break;
      case ELEMENT_TET10:
        mfemToLibmeshMap = (int *)mfemToLibmeshTet10;
        break;
      case ELEMENT_HEX27:
        mfemToLibmeshMap = (int *)mfemToLibmeshHex27;
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

    // Define quadratic FE space
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

        // Sets DOF array of element.
        mfem::Array<int> dofs;
        finite_element_space->GetElementDofs(ielement, dofs);

        // Sets VDOFs array.
        mfem::Array<int> vdofs;
        vdofs.SetSize(dofs.Size());

        for (int l = 0; l < dofs.Size(); l++)
        {
          vdofs[l] = dofs[l];
        }

        finite_element_space->DofsToVDofs(vdofs);

        for (int j = 0; j < dofs.Size(); j++)
        {
          int point_id = node_ids[mfemToLibmeshMap[j] - 1];

          // Map to help with second order variable transfer
          _libmesh_to_mfem_node_map[point_id] = vdofs[j] / 3;

          auto coordinates = coordinates_for_unique_linear_node_id[point_id];

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

const std::map<int, int> &
MFEMMesh::getLibmeshToMFEMNodeMap() const
{
  return _libmesh_to_mfem_node_map;
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
