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
                   std::map<int, std::vector<int>> & elem_blk,
                   int num_blocks_in_mesh,
                   int num_nodes_per_element,
                   std::map<int, size_t> & num_elements_per_block,
                   int num_linaer_nodes_per_element,
                   int num_face_nodes,
                   int num_face_linear_nodes,
                   int num_side_sets,
                   std::vector<int> num_side_in_ss,
                   const std::vector<std::vector<int>> & ss_node_id,
                   const std::vector<int> & unique_block_ids,
                   const std::vector<int> & unique_side_boundary_ids,
                   int num_dimensions,
                   const std::vector<int> & start_of_block)
{

  const int mfemToLibmeshTet10[10] = {1, 2, 3, 4, 5, 7, 8, 6, 9, 10};

  const int mfemToLibmeshHex27[27] = {1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 17, 18,
                                      19, 20, 13, 14, 15, 16, 21, 22, 23, 24, 25, 26, 27};

  const int mfemToLibmeshTri6[6] = {1, 2, 3, 4, 5, 6};
  const int mfemToLibmeshQuad9[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

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

  int elcount = 0;
  int renumberedVertID[8];

  for (int iblock = 0; iblock < num_blocks_in_mesh; iblock++) // for all blocks
  {
    const int block_id = unique_block_ids[iblock];

    for (int jelement = 0; jelement < num_elements_per_block[block_id]; jelement++)
    {
      for (int knode = 0; knode < num_linaer_nodes_per_element; knode++)
      {
        renumberedVertID[knode] = cubit_to_MFEM_vertex_map
            [(elem_blk[block_id][jelement * num_nodes_per_element + knode]) + 1];
        // std::cout << cubitToMFEMVertMap[1] << std::endl;
      }

      switch (libmesh_element_type)
      {
        case ELEMENT_TRI3:
        case ELEMENT_TRI6:
        {
          elements[elcount] = new mfem::Triangle(renumberedVertID, block_id);
          break;
        }
        case ELEMENT_QUAD4:
        case ELEMENT_QUAD9:
        {
          elements[elcount] = new mfem::Quadrilateral(renumberedVertID, block_id);
          break;
        }
        case ELEMENT_TET4:
        case ELEMENT_TET10:
        {
#ifdef MFEM_USE_MEMALLOC
          elements[elcount] = TetMemory.Alloc();
          elements[elcount]->SetVertices(renumberedVertID);
          elements[elcount]->SetAttribute(block_id);
#else
          elements[elcount] = new mfem::Tetrahedron(renumberedVertID, block_id);
#endif
          break;
        }
        case ELEMENT_HEX8:
        case ELEMENT_HEX27:
        {
          elements[elcount] = new mfem::Hexahedron(renumberedVertID, block_id);
          break;
        }
      }

      elcount++;
    }
  }

  NumOfBdrElements = 0;

  // For all the sidesets, add the number of sides to numbdrelems
  for (int iss = 0; iss < num_side_sets; iss++)
  {
    NumOfBdrElements += num_side_in_ss[iss];
  }

  boundary.SetSize(NumOfBdrElements);

  int sidecount = 0;

  for (int iss = 0; iss < num_side_sets; iss++)
  {
    for (int jside = 0; jside < num_side_in_ss[iss]; jside++)
    {
      for (int knode = 0; knode < num_face_linear_nodes; knode++)
      {
        renumberedVertID[knode] =
            cubit_to_MFEM_vertex_map[1 + ss_node_id[iss][jside * num_face_nodes + knode]];
        // std::cout << renumberedVertID[j] << std::endl;
      }

      switch (libmesh_face_type)
      {
        case FACE_EDGE2:
        case FACE_EDGE3:
        {
          boundary[sidecount] = new mfem::Segment(renumberedVertID, unique_side_boundary_ids[iss]);
          break;
        }
        case FACE_TRI3:
        case FACE_TRI6:
        {
          boundary[sidecount] = new mfem::Triangle(renumberedVertID, unique_side_boundary_ids[iss]);
          break;
        }
        case FACE_QUAD4:
        case FACE_QUAD9:
        {
          boundary[sidecount] =
              new mfem::Quadrilateral(renumberedVertID, unique_side_boundary_ids[iss]);
          break;
        }
      }

      sidecount++;
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
    mfem::FiniteElementCollection * fec = new mfem::H1_FECollection(2, 3);
    mfem::FiniteElementSpace * fes =
        new mfem::FiniteElementSpace(this, fec, Dim, mfem::Ordering::byVDIM);

    Nodes = new mfem::GridFunction(fes);
    Nodes->MakeOwner(fec); // Nodes will destroy 'fec' and 'fes'

    own_nodes = 1;

    for (int i = 0; i < NumOfElements; i++)
    {
      mfem::Array<int> dofs;

      fes->GetElementDofs(i, dofs);

      mfem::Array<int> vdofs;
      vdofs.SetSize(dofs.Size());

      for (int l = 0; l < dofs.Size(); l++)
      {
        vdofs[l] = dofs[l];
      }

      fes->DofsToVDofs(vdofs);

      int iblk = 0;

      int loc_ind;

      while (iblk < (int)num_blocks_in_mesh && i >= start_of_block[iblk + 1])
      {
        iblk++;
      }

      loc_ind = i - start_of_block[iblk];

      for (int j = 0; j < dofs.Size(); j++)
      {
        int point_id = elem_blk[unique_block_ids[iblk]]
                               [loc_ind * num_nodes_per_element + mfemToLibmeshMap[j] - 1];

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

  // Finalize mesh method is needed to fully finish constructing the mesh
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
