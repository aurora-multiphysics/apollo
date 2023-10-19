#pragma once
#include "MFEMMesh.h"
#include "MooseError.h"
#include <cstdio>

// Function prototypes:
static bool coordinatesMatch(double primary[3], double secondary[3], const double tolerance = 0.01);
static void convertToCArray(std::array<double, 3> & array_in, double array_out[3]);

MFEMMesh::MFEMMesh(
    const int num_elements_in_mesh,
    const CubitElementInfo & element_info,
    const std::vector<int> & unique_block_ids,
    const std::vector<int> & unique_side_boundary_ids,
    const std::vector<int> & unique_libmesh_corner_node_ids,
    std::map<int, int> & num_elements_for_boundary_id,
    std::map<int, std::vector<int>> & libmesh_element_ids_for_block_id,
    std::map<int, std::vector<int>> & libmesh_node_ids_for_element_id,
    std::map<int, std::vector<int>> & libmesh_node_ids_for_boundary_id,
    std::map<int, std::array<double, 3>> & coordinates_for_libmesh_node_id,
    NodeBiMap * second_order_node_bimap,
    std::map<int, std::vector<int>> * libmesh_center_of_face_node_ids_for_hex27_element_id)
  : _libmesh_element_id_for_mfem_element_id{}, _mfem_vertex_index_for_libmesh_corner_node_id{}
{
  // Set dimensions.
  Dim = spaceDim = element_info.getDimension();

  // Create the vertices.
  buildMFEMVertices(unique_libmesh_corner_node_ids, coordinates_for_libmesh_node_id);

  // Create the mesh elements.
  buildMFEMElements(num_elements_in_mesh,
                    element_info,
                    unique_block_ids,
                    libmesh_element_ids_for_block_id,
                    libmesh_node_ids_for_element_id);

  // Create the boundary elements.
  buildMFEMBoundaryElements(element_info,
                            unique_side_boundary_ids,
                            num_elements_for_boundary_id,
                            libmesh_node_ids_for_boundary_id);

  // Handle higher-order meshes.
  if (element_info.getOrder() == 2)
  {
    if (!second_order_node_bimap)
    {
      mooseError("The second order node bimap is NULL.\n");
    }

    handleQuadraticFESpace(element_info,
                           unique_block_ids,
                           libmesh_element_ids_for_block_id,
                           libmesh_node_ids_for_element_id,
                           coordinates_for_libmesh_node_id,
                           *second_order_node_bimap,
                           libmesh_center_of_face_node_ids_for_hex27_element_id);
  }

  // Finalize mesh method is needed to fully finish constructing the mesh.
  FinalizeMesh();
}

MFEMMesh::MFEMMesh(std::string mesh_fname, int generate_edges, int refine, bool fix_orientation)
  : _libmesh_element_id_for_mfem_element_id{}, _mfem_vertex_index_for_libmesh_corner_node_id{}
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
MFEMMesh::buildMFEMVertices(const std::vector<int> & unique_libmesh_corner_node_ids,
                            std::map<int, std::array<double, 3>> & coordinates_for_libmesh_node_id)
{
  _mfem_vertex_index_for_libmesh_corner_node_id.clear();

  NumOfVertices = unique_libmesh_corner_node_ids.size();
  vertices.SetSize(NumOfVertices);

  // Iterate over the global IDs of each unqiue corner node from the MOOSE mesh.
  const bool use_z_component = (Dim == 3);

  int ivertex = 0;
  for (int libmesh_node_id : unique_libmesh_corner_node_ids)
  {
    // Get the xyz coordinates associated with the libmesh corner node.
    auto & coordinates = coordinates_for_libmesh_node_id[libmesh_node_id];

    // Set xyz components.
    vertices[ivertex](0) = coordinates[0];
    vertices[ivertex](1) = coordinates[1];

    if (use_z_component)
    {
      vertices[ivertex](2) = coordinates[2];
    }

    _mfem_vertex_index_for_libmesh_corner_node_id[libmesh_node_id] = ivertex;

    ivertex++;
  }
}

void
MFEMMesh::buildMFEMElements(const int num_elements_in_mesh,
                            const CubitElementInfo & element_info,
                            const std::vector<int> & unique_block_ids,
                            std::map<int, std::vector<int>> & element_ids_for_block_id,
                            std::map<int, std::vector<int>> & node_ids_for_element_id)
{
  _libmesh_element_id_for_mfem_element_id.clear();

  // Set mesh elements.
  NumOfElements = num_elements_in_mesh;
  elements.SetSize(num_elements_in_mesh);

  // NB: "vertices" and "corner nodes" are equivalent.
  const int num_vertices = element_info.getNumCornerNodes();

  int renumbered_vertex_ids[num_vertices];

  int ielement = 0;
  for (int block_id : unique_block_ids) // Iterate over blocks.
  {
    auto & element_ids = element_ids_for_block_id[block_id];

    for (int element_id : element_ids) // Iterate over elements in block.
    {
      auto & libmesh_node_ids = node_ids_for_element_id[element_id];

      // Iterate over ONLY the corner nodes in the element.
      for (int ivertex = 0; ivertex < num_vertices; ivertex++)
      {
        const int libmesh_node_id = libmesh_node_ids[ivertex];

        // Map from the corner libmesh node --> corresponding mfem vertex.
        renumbered_vertex_ids[ivertex] =
            _mfem_vertex_index_for_libmesh_corner_node_id[libmesh_node_id];
      }

      // Map from mfem element id to libmesh element id.
      _libmesh_element_id_for_mfem_element_id[ielement] = element_id;

      elements[ielement++] =
          buildMFEMElement(element_info.getElementType(), renumbered_vertex_ids, block_id);
    }
  }
}

void
MFEMMesh::buildMFEMBoundaryElements(const CubitElementInfo & element_info,
                                    const std::vector<int> & unique_side_boundary_ids,
                                    std::map<int, int> & num_elements_for_boundary_id,
                                    std::map<int, std::vector<int>> & node_ids_for_boundary_id)
{
  // Set boundary elements:
  NumOfBdrElements = 0;

  for (int boundary_id : unique_side_boundary_ids)
  {
    NumOfBdrElements += num_elements_for_boundary_id[boundary_id];
  }

  boundary.SetSize(NumOfBdrElements);

  const int num_face_vertices = element_info.getNumFaceCornerNodes();

  int renumbered_vertex_ids[num_face_vertices];

  int iboundary = 0;
  for (int boundary_id : unique_side_boundary_ids)
  {
    auto boundary_nodes = node_ids_for_boundary_id[boundary_id];

    for (int jelement = 0; jelement < num_elements_for_boundary_id[boundary_id]; jelement++)
    {
      for (int knode = 0; knode < num_face_vertices; knode++)
      {
        const int libmesh_node_id =
            boundary_nodes[jelement * element_info.getNumFaceNodes() + knode];

        // Renumber vertex ("node") IDs so they're contiguous and start from 0.
        renumbered_vertex_ids[knode] =
            _mfem_vertex_index_for_libmesh_corner_node_id[libmesh_node_id];
      }

      boundary[iboundary++] =
          buildMFEMFaceElement(element_info.getFaceType(), renumbered_vertex_ids, boundary_id);
    }
  }
}

mfem::Element *
MFEMMesh::buildMFEMElement(const int element_type, const int * vertex_ids, const int block_id)
{
  mfem::Element * new_element = nullptr;

  switch (element_type)
  {
    case CubitElementInfo::ELEMENT_TRI3:
    case CubitElementInfo::ELEMENT_TRI6:
    {
      new_element = new mfem::Triangle(vertex_ids, block_id);
      break;
    }
    case CubitElementInfo::ELEMENT_QUAD4:
    case CubitElementInfo::ELEMENT_QUAD9:
    {
      new_element = new mfem::Quadrilateral(vertex_ids, block_id);
      break;
    }
    case CubitElementInfo::ELEMENT_TET4:
    case CubitElementInfo::ELEMENT_TET10:
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
    case CubitElementInfo::ELEMENT_HEX8:
    case CubitElementInfo::ELEMENT_HEX27:
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
    case CubitElementInfo::FACE_EDGE2:
    case CubitElementInfo::FACE_EDGE3:
    {
      new_face = new mfem::Segment(vertex_ids, boundary_id);
      break;
    }
    case CubitElementInfo::FACE_TRI3:
    case CubitElementInfo::FACE_TRI6:
    {
      new_face = new mfem::Triangle(vertex_ids, boundary_id);
      break;
    }
    case CubitElementInfo::FACE_QUAD4:
    case CubitElementInfo::FACE_QUAD9:
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
    const CubitElementInfo & element_info,
    const std::vector<int> & unique_block_ids,
    std::map<int, std::vector<int>> & libmesh_element_ids_for_block_id,
    std::map<int, std::vector<int>> & libmesh_node_ids_for_element_id,
    std::map<int, std::array<double, 3>> & coordinates_for_libmesh_node_id,
    NodeBiMap & second_order_node_bimap,
    std::map<int, std::vector<int>> * libmesh_center_of_face_node_ids_for_hex27_element_id)
{
  // Verify that this is indeed a second-order element.
  if (element_info.getOrder() != 2)
  {
    return;
  }

  // Add a warning for 2D second-order elements but proceed.
  if (element_info.getDimension() == 2)
  {
    mooseWarning("'", __func__, "' has not been tested with second-order 2D elements.");
  }

  // 2D maps:
  const int mfem_to_libmesh_tri6[] = {1, 2, 3, 4, 5, 6};
  const int mfem_to_libmesh_quad9[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

  // 3D maps:
  const int mfem_to_libmesh_tet10[] = {1, 2, 3, 4, 5, 7, 8, 6, 9, 10};
  const int mfem_to_libmesh_hex27[] = {1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 17, 18,
                                       19, 20, 13, 14, 15, 16, 22, 26, 25, 27, 24, 23, 21};

  int * mfem_to_libmesh_map = nullptr;

  switch (element_info.getElementType())
  {
    case CubitElementInfo::ELEMENT_TRI6:
    {
      mfem_to_libmesh_map = (int *)mfem_to_libmesh_tri6;
      break;
    }
    case CubitElementInfo::ELEMENT_QUAD9:
    {
      mfem_to_libmesh_map = (int *)mfem_to_libmesh_quad9;
      break;
    }
    case CubitElementInfo::ELEMENT_TET10:
    {
      mfem_to_libmesh_map = (int *)mfem_to_libmesh_tet10;
      break;
    }
    case CubitElementInfo::ELEMENT_HEX27:
    {
      mfem_to_libmesh_map = (int *)mfem_to_libmesh_hex27;
      break;
    }
    default:
    {
      mooseError("No second-order map available for element type ",
                 element_info.getElementType(),
                 " with dimension ",
                 element_info.getDimension(),
                 ".\n");
      break;
    }
  }

  /*
  Call FinalizeTopology. If we call this then we must call Finalize later after
  we've defined the mesh nodes.
  */
  FinalizeTopology();

  // Define quadratic FE space.
  mfem::FiniteElementCollection * finite_element_collection = new mfem::H1_FECollection(2, 3);

  // NB: the specified ordering is byVDIM.
  // byVDim: XYZ, XYZ, XYZ, XYZ,...
  // byNode: XXX..., YYY..., ZZZ...
  mfem::FiniteElementSpace * finite_element_space =
      new mfem::FiniteElementSpace(this, finite_element_collection, Dim, mfem::Ordering::byVDIM);

  Nodes = new mfem::GridFunction(finite_element_space);
  Nodes->MakeOwner(finite_element_collection); // Nodes will destroy 'finite_element_collection'
  own_nodes = 1;                               // and 'finite_element_space'

  // Clear second order node bimap.
  second_order_node_bimap.clearNodeIDs();

  for (int ielement = 0; ielement < NumOfElements; ielement++)
  {
    const int libmesh_element_id = _libmesh_element_id_for_mfem_element_id[ielement];

    // Get vector containing ALL node global IDs for element.
    auto & libmesh_node_ids = libmesh_node_ids_for_element_id[libmesh_element_id];

    // Sets DOF array for element. Higher-order (second-order) elements contain
    // additional nodes between corner nodes.
    mfem::Array<int> dofs;
    finite_element_space->GetElementDofs(ielement, dofs);

    // NB: returned indices are ALWAYS ordered byNodes (ie xxx..., yyy..., zzz...)
    mfem::Array<int> vdofs;
    finite_element_space->GetElementVDofs(ielement, vdofs);

    // Iterate over dofs array.
    for (int j = 0; j < dofs.Size(); j++)
    {
      const int mfem_node_id = dofs[j];

      // Find the libmesh node ID:
      // NB: the map is 1-based to we need to subtract 1.
      const int libmesh_node_index = mfem_to_libmesh_map[j] - 1;
      const int libmesh_node_id = libmesh_node_ids[libmesh_node_index];

      // Update two-way map:
      second_order_node_bimap.insertNodeIDs(mfem_node_id, libmesh_node_id);

      // Extract node's coordinates:
      auto & coordinates = coordinates_for_libmesh_node_id[libmesh_node_id];

      // NB: vdofs using xxx, yyy, zzz ordering.
      (*Nodes)(vdofs[j]) = coordinates[0];
      (*Nodes)(vdofs[j + dofs.Size()]) = coordinates[1];

      if (Dim == 3)
      {
        (*Nodes)(vdofs[j + 2 * dofs.Size()]) = coordinates[2];
      }
    }
  }

  /**
   * Fix the elements for hex27 only. The centre node of each face is incorrect
   * in addition to the interior node.
   */
  if (element_info.getElementType() == CubitElementInfo::ELEMENT_HEX27)
  {
    // Verify that we have the map. If not, we cannot apply the correction.
    // Ensure that we have the face map for hex27 correction.
    if (!libmesh_center_of_face_node_ids_for_hex27_element_id)
    {
      mooseError("Cannot apply hex27 element correction due to NULL map!\n");
    }

    fixHex27MeshNodes(*finite_element_space,
                      coordinates_for_libmesh_node_id,
                      libmesh_node_ids_for_element_id,
                      *libmesh_center_of_face_node_ids_for_hex27_element_id,
                      second_order_node_bimap);
  }

  /**
   * Ensure that there is a one-to-one mapping between libmesh and mfem node ids.
   * All coordinates should match. If this does not occur then it suggests that
   * there is a problem with the higher-order transfer.
   */
  verifyUniqueMappingBetweenLibmeshAndMFEMNodes(*finite_element_space,
                                                unique_block_ids,
                                                libmesh_element_ids_for_block_id,
                                                libmesh_node_ids_for_element_id,
                                                coordinates_for_libmesh_node_id,
                                                second_order_node_bimap);
}

void
MFEMMesh::verifyUniqueMappingBetweenLibmeshAndMFEMNodes(
    mfem::FiniteElementSpace & finite_element_space,
    const std::vector<int> & unique_block_ids,
    std::map<int, std::vector<int>> & libmesh_element_ids_for_block_id,
    std::map<int, std::vector<int>> & libmesh_node_ids_for_element_id,
    std::map<int, std::array<double, 3>> & coordinates_for_libmesh_node_id,
    NodeBiMap & second_order_node_bimap)
{
  // Create a set of all unique libmesh node ids.
  std::set<int> libmesh_node_ids;

  for (auto & key_value : libmesh_node_ids_for_element_id)
  {
    std::vector<int> & libmesh_node_ids_for_element = key_value.second;

    for (int libmesh_node_id : libmesh_node_ids_for_element)
    {
      libmesh_node_ids.insert(libmesh_node_id);
    }
  }

  double mfem_coordinates[3], libmesh_coordinates[3];

  for (int ielement = 0; ielement < NumOfElements; ielement++)
  {
    mfem::Array<int> mfem_dofs;
    finite_element_space.GetElementDofs(ielement, mfem_dofs);

    for (int mfem_dof : mfem_dofs)
    {
      GetNode(mfem_dof, mfem_coordinates);

      const int libmesh_node_id = second_order_node_bimap.getLibmeshNodeID(mfem_dof);

      // Remove from set.
      libmesh_node_ids.erase(libmesh_node_id);

      // Convert from std::array<double, 3> --> C array for comparison.
      auto & coordinates = coordinates_for_libmesh_node_id[libmesh_node_id];
      convertToCArray(coordinates, libmesh_coordinates);

      if (!coordinatesMatch(libmesh_coordinates, mfem_coordinates))
      {
        mooseError("Non-matching coordinates detected for libmesh node ",
                   libmesh_node_id,
                   " and MFEM node ",
                   mfem_dof,
                   " for MFEM element ",
                   ielement,
                   ".\n");
      }
    }
  }

  // Check how many elements remain in set of libmesh element ids. Ideally,
  // there should be none left indicating that we've referenced every single
  // element in the set.
  if (libmesh_node_ids.size() != 0)
  {
    mooseError("There are ",
               libmesh_node_ids.size(),
               " unpaired libmesh node ids. No one-to-one mapping exists!\n");
  }
}

void
MFEMMesh::fixHex27MeshNodes(
    mfem::FiniteElementSpace & finite_element_space,
    std::map<int, std::array<double, 3>> & coordinates_for_libmesh_node_id,
    std::map<int, std::vector<int>> & libmesh_node_ids_for_element_id,
    std::map<int, std::vector<int>> & libmesh_center_of_face_node_ids_for_hex27_element_id,
    NodeBiMap & second_order_node_bimap)
{
  /**
   * Problems with hex27 meshes:
   * 1) the middle node from each face (3x3) is incorrect
   * 2) the interior central node is incorrect
   */
  for (int ielement = 0; ielement < NumOfElements; ielement++)
  {
    //
    // 1) Face correction:
    //
    // Get the corresponding libmesh element.
    auto libmesh_element_id = _libmesh_element_id_for_mfem_element_id[ielement];
    auto & libmesh_node_ids = libmesh_node_ids_for_element_id[libmesh_element_id];

    auto & libmesh_center_of_face_node_ids =
        libmesh_center_of_face_node_ids_for_hex27_element_id[ielement];

    // Sanity check: there should be 27 libmesh node ids.
    if (libmesh_node_ids.size() != 27)
    {
      mooseError("There should be 27 nodes per libmesh element for hex27 elements!\n");
    }

    // Iterate over faces and get the dofs for each face of the element.
    mfem::Array<int> faces;
    mfem::Array<int> face_orientations;

    GetElementFaces(ielement, faces, face_orientations);

    // Sanity check: there should be 6 faces:
    if (faces.Size() != 6)
    {
      mooseError("There should be 6 faces for hex27 elements!\n");
    }

    for (int iface = 0; iface < faces.Size(); iface++)
    {
      const int mfem_face = faces[iface];

      mfem::Array<int> face_dofs;
      finite_element_space.GetFaceDofs(mfem_face, face_dofs);

      // NB: the middle node from each face corresponds to the last dof from the
      // faces array.
      const int hex27_face_middle_dof = face_dofs.Last();

      mfem::Array<int> face_vdofs;
      finite_element_space.GetFaceVDofs(mfem_face, face_vdofs);

      // Get the positions in the vdofs vector for the xyz coordinates:
      const int x_vdof_offset = face_dofs.Size() - 1;
      const int y_vdof_offset = x_vdof_offset + face_dofs.Size();
      const int z_vdof_offset = y_vdof_offset + face_dofs.Size();

      const int x_node_offset = face_vdofs[x_vdof_offset];
      const int y_node_offset = face_vdofs[y_vdof_offset];
      const int z_node_offset = face_vdofs[z_vdof_offset];

      // The middle node from each face is also the last face node in each libmesh
      // vector of node ids.
      auto libmesh_hex27_center_of_face_node_id = libmesh_center_of_face_node_ids[iface];

      // Extract the correct coordinates:
      auto coordinates_for_hex27_face_middle =
          coordinates_for_libmesh_node_id[libmesh_hex27_center_of_face_node_id];

      const double x = coordinates_for_hex27_face_middle[0];
      const double y = coordinates_for_hex27_face_middle[1];
      const double z = coordinates_for_hex27_face_middle[2];

      // Set the correct coordinates for the Node:
      (*Nodes)(x_node_offset) = x;
      (*Nodes)(y_node_offset) = y;
      (*Nodes)(z_node_offset) = z;

      // Correct the maps:
      second_order_node_bimap.insertNodeIDs(hex27_face_middle_dof,
                                            libmesh_hex27_center_of_face_node_id);
    }

    //
    // 2) Interior (central) node correction:
    //
    mfem::Array<int> interior_dofs;
    finite_element_space.GetElementInteriorDofs(ielement, interior_dofs);

    mfem::Array<int> interior_vdofs;
    finite_element_space.GetElementInteriorVDofs(ielement, interior_vdofs);

    // Sanity check: there should be ONLY 1 interior node.
    if (interior_dofs.Size() != 1)
    {
      mooseError("There should be a single interior node for hex27 elements!\n");
      return;
    }

    // Get the positions in the vdofs vector for the xyz coordinates:
    const int x_vdof_offset = 0;
    const int y_vdof_offset = x_vdof_offset + interior_dofs.Size();
    const int z_vdof_offset = y_vdof_offset + interior_dofs.Size();

    const int x_node_offset = interior_vdofs[x_vdof_offset];
    const int y_node_offset = interior_vdofs[y_vdof_offset];
    const int z_node_offset = interior_vdofs[z_vdof_offset];

    // NB: the last libmesh node id corresponds to the central node.
    const int libmesh_hex27_interior_node_id = libmesh_node_ids.back();

    // Extract the correct coordinates:
    auto & coordinates_for_hex27_interior =
        coordinates_for_libmesh_node_id[libmesh_hex27_interior_node_id];

    const double x = coordinates_for_hex27_interior[0];
    const double y = coordinates_for_hex27_interior[1];
    const double z = coordinates_for_hex27_interior[2];

    // Set the correct coordinates for the Nodes:
    (*Nodes)(x_node_offset) = x;
    (*Nodes)(y_node_offset) = y;
    (*Nodes)(z_node_offset) = z;

    // Correct the maps:
    second_order_node_bimap.insertNodeIDs(interior_dofs[0], libmesh_hex27_interior_node_id);
  }
}

void
MFEMMesh::writeFaceInfoFor2ndOrderElements(const char * fpath,
                                           mfem::FiniteElementSpace * fe_space) const
{
  if (!fpath || !fe_space)
  {
    return;
  }

  FILE * fp = fopen(fpath, "w");
  if (!fp)
  {
    return;
  }

  for (int ielement = 0; ielement < NumOfElements; ielement++)
  {
    writeFaceInfoFor2ndOrderElement(fp, fe_space, ielement);
  }

  fclose(fp);
}

void
MFEMMesh::writeNodeInfoFor2ndOrderElements(const char * fpath,
                                           mfem::FiniteElementSpace * fe_space) const
{
  if (!fpath || !fe_space)
  {
    return;
  }

  FILE * fp = fopen(fpath, "w");
  if (!fp)
  {
    return;
  }

  for (int ielement = 0; ielement < NumOfElements; ielement++)
  {
    writeNodeInfoFor2ndOrderElement(fp, fe_space, ielement);
  }

  fclose(fp);
}

void
MFEMMesh::writeEdgeInfoFor2ndOrderElements(const char * fpath,
                                           mfem::FiniteElementSpace * fe_space) const
{
  if (!fpath || !fe_space)
  {
    return;
  }

  FILE * fp = fopen(fpath, "w");
  if (!fp)
  {
    return;
  }

  for (int ielement = 0; ielement < NumOfElements; ielement++)
  {
    writeEdgeInfoFor2ndOrderElement(fp, fe_space, ielement);
  }

  fclose(fp);
}

void
MFEMMesh::writeFaceInfoFor2ndOrderElement(FILE * fp,
                                          mfem::FiniteElementSpace * fe_space,
                                          const int ielement) const
{
  if (!fp || !fe_space)
  {
    return;
  }

  fprintf(fp, "*** Element %d ***\n", ielement);

  mfem::Array<int> faces;
  mfem::Array<int> face_orientations;

  GetElementFaces(ielement, faces, face_orientations);

  for (int iface = 0; iface < faces.Size(); iface++)
  {
    fprintf(fp, "\t*** Face %d ***\n", iface);

    mfem::Array<int> face_dofs;
    fe_space->GetFaceDofs(faces[iface], face_dofs);

    for (int face_dof : face_dofs)
    {
      double coordinates[3];
      GetNode(face_dof, coordinates);

      fprintf(fp, "\t\t(%.2lf, %.2lf, %.2lf)\n", coordinates[0], coordinates[1], coordinates[2]);
    }
  }
}

void
MFEMMesh::writeNodeInfoFor2ndOrderElement(FILE * fp,
                                          mfem::FiniteElementSpace * fe_space,
                                          const int ielement) const
{
  if (!fp || !fe_space)
  {
    return;
  }

  fprintf(fp, "*** Element %d ***\n", ielement);

  mfem::Array<int> node_dofs;
  fe_space->GetElementDofs(ielement, node_dofs);

  for (int node_dof : node_dofs)
  {
    double coordinates[3];
    GetNode(node_dof, coordinates);

    fprintf(fp, "\t(%.2lf, %.2lf, %.2lf)\n", coordinates[0], coordinates[1], coordinates[2]);
  }
}

void
MFEMMesh::writeEdgeInfoFor2ndOrderElement(FILE * fp,
                                          mfem::FiniteElementSpace * fe_space,
                                          const int ielement) const
{
  if (!fp || !fe_space)
  {
    return;
  }

  fprintf(fp, "*** Element %d ***\n", ielement);

  mfem::Array<int> edges;
  mfem::Array<int> edge_orientations;

  GetElementEdges(ielement, edges, edge_orientations);

  for (int iedge = 0; iedge < edges.Size(); iedge++)
  {
    fprintf(fp, "\t*** Edge %d ***\n", iedge);

    mfem::Array<int> edge_dofs;
    fe_space->GetEdgeDofs(edges[iedge], edge_dofs);

    for (int edge_dof : edge_dofs)
    {
      double coordinates[3];
      GetNode(edge_dof, coordinates);

      fprintf(fp, "\t\t(%.2lf, %.2lf, %.2lf)\n", coordinates[0], coordinates[1], coordinates[2]);
    }
  }
}

static bool
coordinatesMatch(double primary[3], double secondary[3], const double tolerance)
{
  if (!primary || !secondary || tolerance < 0.0)
  {
    return false;
  }

  for (int i = 0; i < 3; i++)
  {
    if (fabs(primary[i] - secondary[i]) > tolerance)
    {
      return false;
    }
  }

  return true;
}

static void
convertToCArray(std::array<double, 3> & array_in, double array_out[3])
{
  if (!array_out)
  {
    return;
  }

  for (int i = 0; i < 3; i++)
  {
    array_out[i] = array_in[i];
  }
}
