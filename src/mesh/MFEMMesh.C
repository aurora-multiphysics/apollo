#pragma once
#include "MFEMMesh.h"
#include "MooseError.h"

// Constructor to create an MFEM mesh from VTK data structures. These data
// structures are obtained by the methods found in MFEMproblem
MFEMMesh::MFEMMesh(int num_elements_in_mesh,
                   std::vector<double> & coordx,
                   std::vector<double> & coordy,
                   std::vector<double> & coordz,
                   std::map<int, int> & cubit_to_MFEM_vertex_map,
                   std::vector<int> unique_vertex_ids,
                   int libmesh_element_type,
                   int libmesh_face_type,
                   std::map<int, std::vector<int>> element_ids_for_block_id,
                   std::map<int, std::vector<int>> node_ids_for_element_id,
                   std::map<int, int> & block_id_for_element_id,
                   std::map<int, std::vector<int>> & element_nodes_for_block_id,
                   int num_nodes_per_element,
                   std::map<int, size_t> & num_elements_per_block,
                   int num_linear_nodes_per_element,
                   int num_face_nodes,
                   int num_face_linear_nodes,
                   std::map<int, int> & num_elements_for_boundary_id,
                   std::map<int, std::vector<int>> & node_ids_for_boundary_id,
                   const std::vector<int> & unique_block_ids,
                   const std::vector<int> & unique_side_boundary_ids,
                   int num_dimensions,
                   std::map<int, int> & start_of_block)
{

  const int mfemToLibmeshTet10[] = {1, 2, 3, 4, 5, 7, 8, 6, 9, 10};

  const int mfemToLibmeshHex27[] = {1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 17, 18,
                                    19, 20, 13, 14, 15, 16, 21, 22, 23, 24, 25, 26, 27};

  const int mfemToLibmeshTri6[] = {1, 2, 3, 4, 5, 6};
  const int mfemToLibmeshQuad9[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

  const int order = getOrderFromLibmeshElementType(libmesh_element_type);

  Dim = num_dimensions;
  spaceDim = Dim;
  NumOfElements = num_elements_in_mesh;
  NumOfVertices = unique_vertex_ids.size();

  vertices.SetSize(NumOfVertices);
  elements.SetSize(num_elements_in_mesh);

  for (std::size_t i = 0; i < unique_vertex_ids.size(); i++)
  {
    vertices[i](0) = coordx[unique_vertex_ids[i] - 1];
    vertices[i](1) = coordy[unique_vertex_ids[i] - 1];

    if (Dim == 3)
    {
      vertices[i](2) = coordz[unique_vertex_ids[i] - 1];
    }
  }

  int element_counter = 0;

  int renumbered_vertex_ids[8];

  for (int block_id : unique_block_ids)
  {
    auto & element_ids = element_ids_for_block_id[block_id];

    for (int element_id : element_ids)
    {
      auto & node_ids = node_ids_for_element_id[element_id];

      for (int node_index = 0; node_index < num_linear_nodes_per_element; node_index++)
      {
        renumbered_vertex_ids[node_index] = cubit_to_MFEM_vertex_map[node_ids[node_index] + 1];
      }

      switch (libmesh_element_type)
      {
        case ELEMENT_TRI3:
        case ELEMENT_TRI6:
        {
          elements[element_counter] = new mfem::Triangle(renumbered_vertex_ids, block_id);
          break;
        }
        case ELEMENT_QUAD4:
        case ELEMENT_QUAD9:
        {
          elements[element_counter] = new mfem::Quadrilateral(renumbered_vertex_ids, block_id);
          break;
        }
        case ELEMENT_TET4:
        case ELEMENT_TET10:
        {
#ifdef MFEM_USE_MEMALLOC
          elements[element_counter] = TetMemory.Alloc();
          elements[element_counter]->SetVertices(renumbered_vertex_ids);
          elements[element_counter]->SetAttribute(block_id);
#else
          elements[elcount] = new mfem::Tetrahedron(renumbered_vertex_ids, block_id);
#endif
          break;
        }
        case ELEMENT_HEX8:
        case ELEMENT_HEX27:
        {
          elements[element_counter] = new mfem::Hexahedron(renumbered_vertex_ids, block_id);
          break;
        }
      }

      element_counter++;
    }
  }

  NumOfBdrElements = 0;

  for (int boundary_id : unique_side_boundary_ids)
  {
    NumOfBdrElements += num_elements_for_boundary_id[boundary_id];
  }

  boundary.SetSize(NumOfBdrElements);

  int boundary_counter = 0;

  for (int boundary_id : unique_side_boundary_ids)
  {
    auto boundary_nodes = node_ids_for_boundary_id[boundary_id];

    for (int jelement = 0; jelement < num_elements_for_boundary_id[boundary_id]; jelement++)
    {
      for (int knode = 0; knode < num_face_linear_nodes; knode++)
      {
        const int node_global_index = boundary_nodes[jelement * num_face_nodes + knode];

        renumbered_vertex_ids[knode] = cubit_to_MFEM_vertex_map[1 + node_global_index];
      }

      switch (libmesh_face_type)
      {
        case FACE_EDGE2:
        case FACE_EDGE3:
        {
          boundary[boundary_counter] = new mfem::Segment(renumbered_vertex_ids, boundary_id);
          break;
        }
        case FACE_TRI3:
        case FACE_TRI6:
        {
          boundary[boundary_counter] = new mfem::Triangle(renumbered_vertex_ids, boundary_id);
          break;
        }
        case FACE_QUAD4:
        case FACE_QUAD9:
        {
          boundary[boundary_counter] = new mfem::Quadrilateral(renumbered_vertex_ids, boundary_id);
          break;
        }
      }

      boundary_counter++;
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

    own_nodes = 1; // True.

    // No! These may not be the correct element IDs! MFEM expects element_ids to be incremental
    // from 0!!! Think about!!!
    for (int ielement = 0; ielement < NumOfElements; ielement++)
    {
      mfem::Array<int> dofs;
      finite_element_space->GetElementDofs(ielement, dofs);

      mfem::Array<int> vdofs;
      vdofs.SetSize(dofs.Size());

      for (int l = 0; l < dofs.Size(); l++)
      {
        vdofs[l] = dofs[l];
      }

      finite_element_space->DofsToVDofs(vdofs);

      // int block_index = 0;

      // Locate which block the element originates from. TODO: - a reverse map would work here...
      // const int num_blocks_in_mesh = unique_block_ids.size();

      // while (block_index < (num_blocks_in_mesh - 1) &&
      //        ielement >= start_of_block[unique_block_ids[block_index + 1]])
      // {
      //   block_index++;
      // }

      const int block_id = block_id_for_element_id[ielement];

      // const int block_id = unique_block_ids[block_index];

      const int element_offset = ielement - start_of_block[block_id];

      for (int j = 0; j < dofs.Size(); j++)
      {
        int point_id = element_nodes_for_block_id[block_id][element_offset * num_nodes_per_element +
                                                            mfemToLibmeshMap[j] - 1];

        // Map to help with second order variable transfer
        _libmesh_to_mfem_node_map[point_id] = vdofs[j] / 3;

        (*Nodes)(vdofs[j]) = coordx[point_id];
        (*Nodes)(vdofs[j] + 1) = coordy[point_id];

        if (Dim == 3)
        {
          (*Nodes)(vdofs[j] + 2) = coordz[point_id];
        }
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
