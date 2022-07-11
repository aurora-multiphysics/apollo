#pragma once
#include "MFEMMesh.h"

// Constructor to create an MFEM mesh from VTK data structures. These data
// structures are obtained by the methods found in MFEMproblem
MFEMMesh::MFEMMesh(
    int num_elem, std::vector<double> &coordx, std::vector<double> &coordy,
    std::vector<double> &coordz, std::map<int, int> &cubitToMFEMVertMap,
    std::vector<int> uniqueVertexID, int libmesh_element_type,
    int libmesh_face_type, int **elem_blk, int num_el_blk, int num_node_per_el,
    size_t *num_el_in_blk, int num_element_linear_nodes, int num_face_nodes,
    int num_face_linear_nodes, int num_side_sets,
    std::vector<int> num_side_in_ss, int **ss_node_id, int *ebprop, int *ssprop,
    int dim_num, int* start_of_block) {



  const int mfemToGenesisTet10[10] = {1, 2, 3, 4, 5, 7, 8, 6, 9, 10};

  //                                  1,2,3,4,5,6,7,8,9,10,11,
  const int mfemToGenesisHex27[27] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
                                      // 12,13,14,15,16,17,18,19
                                      12, 17, 18, 19, 20, 13, 14, 15,
                                      // 20,21,22,23,24,25,26,27
                                      16, 21, 22, 23, 24, 25, 26, 27};

  const int mfemToGenesisTri6[6] = {1, 2, 3, 4, 5, 6};
  const int mfemToGenesisQuad9[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};


  int order = 0;
  if (libmesh_element_type == ELEMENT_TRI3 || libmesh_element_type == ELEMENT_QUAD4 ||
      libmesh_element_type == ELEMENT_TET4 || libmesh_element_type == ELEMENT_HEX8)
  {
    order = 1;
  }
  else if (libmesh_element_type == ELEMENT_TRI6 ||
          libmesh_element_type == ELEMENT_QUAD9 ||
          libmesh_element_type == ELEMENT_TET10 || libmesh_element_type == ELEMENT_HEX27)
  {
    order = 2;
  }      
  Dim = dim_num;
  spaceDim = Dim;
  NumOfElements = num_elem;
  NumOfVertices = uniqueVertexID.size();
  vertices.SetSize(NumOfVertices);
  elements.SetSize(num_elem);
  for (int i = 0; i < (int)uniqueVertexID.size(); i++) {
    vertices[i](0) = coordx[uniqueVertexID[i]-1];
    vertices[i](1) = coordy[uniqueVertexID[i]-1];
    if (Dim == 3) {
      vertices[i](2) = coordz[uniqueVertexID[i]-1];
    }
  }


  int elcount = 0;
  int renumberedVertID[8];
  for (int iblk = 0; iblk < (int)num_el_blk; iblk++)  // for all blocks
  {
    int NumNodePerEl = num_node_per_el;
    for (int i = 0; i < (int)num_el_in_blk[iblk]; i++) {
      for (int j = 0; j < num_element_linear_nodes; j++) {
        renumberedVertID[j] =
            cubitToMFEMVertMap[(elem_blk[iblk][i * NumNodePerEl + j]) + 1];
      }

      switch (libmesh_element_type) {
        case (ELEMENT_TRI3):
        case (ELEMENT_TRI6): {
          elements[elcount] =
              new mfem::Triangle(renumberedVertID, ebprop[iblk]);
          break;
        }
        case (ELEMENT_QUAD4):
        case (ELEMENT_QUAD9): {
          elements[elcount] =
              new mfem::Quadrilateral(renumberedVertID, ebprop[iblk]);
          break;
        }
        case (ELEMENT_TET4):
        case (ELEMENT_TET10): {
#ifdef MFEM_USE_MEMALLOC
          elements[elcount] = TetMemory.Alloc();
          elements[elcount]->SetVertices(renumberedVertID);
          elements[elcount]->SetAttribute(ebprop[iblk]);
#else
          elements[elcount] =
              new mfem::Tetrahedron(renumberedVertID, ebprop[iblk]);
#endif
          break;
        }
        case (ELEMENT_HEX8):
        case (ELEMENT_HEX27): {
          elements[elcount] =
              new mfem::Hexahedron(renumberedVertID, ebprop[iblk]);
          break;
        }
      }
      elcount++;
    }
  }
  NumOfBdrElements = 0;
  for (int iss = 0; iss < (int)num_side_sets;
       iss++)  // For all the sidesets, add the number of sides to numbdrelems
  {
    NumOfBdrElements += num_side_in_ss[iss];
  }
  boundary.SetSize(NumOfBdrElements);
  int sidecount = 0;
  for (int iss = 0; iss < (int)num_side_sets; iss++) {
    for (int i = 0; i < (int)num_side_in_ss[iss]; i++) {
      for (int j = 0; j < num_face_linear_nodes; j++) {
        renumberedVertID[j] =
            cubitToMFEMVertMap[1 + ss_node_id[iss][i * num_face_nodes + j]];
      }
      switch (libmesh_face_type) {
        case (FACE_EDGE2):
        case (FACE_EDGE3): {
          boundary[sidecount] =
              new mfem::Segment(renumberedVertID, ssprop[iss]);
          break;
        }
        case (FACE_TRI3):
        case (FACE_TRI6): {
          boundary[sidecount] =
              new mfem::Triangle(renumberedVertID, ssprop[iss]);
          break;
        }
        case (FACE_QUAD4):
        case (FACE_QUAD9): {
          boundary[sidecount] =
              new mfem::Quadrilateral(renumberedVertID, ssprop[iss]);
          break;
        }
      }
      sidecount++;
    }
  }


  if (order == 2)
  {
    int *mymap = NULL;
    switch (libmesh_element_type)
    {
      case (ELEMENT_TRI6):
      {
        mymap = (int *) mfemToGenesisTri6;
        break;
      }
      case (ELEMENT_QUAD9):
      {
        mymap = (int *) mfemToGenesisQuad9;
        break;
      }
      case (ELEMENT_TET10):
      {
        mymap = (int *) mfemToGenesisTet10;
        break;
      }
      case (ELEMENT_HEX27):
      {
        mymap = (int *) mfemToGenesisHex27;
        break;
      }
      case (ELEMENT_TRI3):
      case (ELEMENT_QUAD4):
      case (ELEMENT_TET4):
      case (ELEMENT_HEX8):
      {
        MFEM_ABORT("Something went wrong. Linear elements detected when order is 2.");
        break;
      }
    }
    std::cout << "Finalising Topology" << std::endl;
    FinalizeTopology();
    // Define quadratic FE space
    mfem::FiniteElementCollection *fec = new mfem::H1_FECollection(2,3);
    mfem::FiniteElementSpace *fes = new mfem::FiniteElementSpace(this, fec, Dim,
                                                      mfem::Ordering::byVDIM);
    Nodes = new mfem::GridFunction(fes);
    Nodes->MakeOwner(fec); // Nodes will destroy 'fec' and 'fes'
    own_nodes = 1;
    
    for (int i = 0; i < NumOfElements; i++)
    {
      mfem::Array<int> dofs;

      fes->GetElementDofs(i, dofs);
      mfem::Array<int> vdofs;
      vdofs.SetSize(dofs.Size());
      for (int l = 0; l < dofs.Size(); l++) { vdofs[l] = dofs[l]; }
      fes->DofsToVDofs(vdofs);
      int iblk = 0;
      int loc_ind;
      while (iblk < (int) num_el_blk && i >= start_of_block[iblk+1]) { iblk++; }
      loc_ind = i - start_of_block[iblk];
      for (int j = 0; j < dofs.Size(); j++)
      {
        int point_id = elem_blk[iblk][loc_ind*num_node_per_el + mymap[j] - 1];
        std::cout << point_id << std::endl;
        (*Nodes)(vdofs[j]) = coordx[point_id];
        (*Nodes)(vdofs[j]+1) = coordy[point_id];
        if (Dim == 3)
        {
            (*Nodes)(vdofs[j]+2) = coordz[point_id];
        }
      }
    }
  }

  std::ofstream mesh_ofs("hello.vtk");
  mesh_ofs.precision(8);
  this->PrintVTK(mesh_ofs);
}

MFEMMesh::MFEMMesh(std::string afilename, int generate_edges,
                                     int refine, bool fix_orientation) {
  std::cout << afilename << std::endl;
  const char *filename = afilename.c_str();
  SetEmpty();

  mfem::named_ifgzstream imesh(filename);
  if (!imesh) {
    // Abort with an error message.
    MFEM_ABORT("Mesh file not found: " << filename << '\n');
  } else {
    Load(imesh, generate_edges, refine, fix_orientation);
  }

}
