#pragma once
#include "MFEMMesh.h"
#include "MooseError.h"
#include <cstdio>

static bool CoordinatesMatch(double primary[3], double secondary[3]);

MFEMMesh::MFEMMesh(
    const int num_dimensions,
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
    std::map<int, std::vector<std::vector<int>>> & global_face_node_ids_for_element_id,
    //  std::map<int, int> & libmesh_to_mfem_corner_node_id_map,
    std::map<int, std::vector<int>> & element_ids_for_block_id,
    std::map<int, std::vector<int>> & node_ids_for_element_id,
    std::map<int, std::vector<int>> & node_ids_for_boundary_id,
    std::map<int, std::array<double, 3>> & coordinates_for_unique_corner_node_id,
    std::map<int, int> & mfem_dof_for_libmesh_node_id)
  : _libmesh_element_id_for_mfem_element_id{},
    _mfem_vertex_index_for_libmesh_corner_node_id{},
    _libmesh_global_face_node_ids_for_element_id{global_face_node_ids_for_element_id}
{
  // Set dimensions.
  Dim = num_dimensions;
  spaceDim = Dim; // Is this always the case?

  // Create the vertices.
  buildMFEMVertices(unique_corner_node_ids, coordinates_for_unique_corner_node_id, num_dimensions);

  if (false)
  {
    FILE * fp = fopen("/opt/mfem_corner_node_ids.txt", "w");
    if (!fp)
      fprintf(stderr, "Error: failed to open file.\n");

    for (int ivertex = 0; ivertex < NumOfVertices; ivertex++)
    {
      double x = vertices[ivertex](0);
      double y = vertices[ivertex](1);
      double z = vertices[ivertex](2);

      fprintf(fp, "(%.2lf, %.2lf, %.2lf)\n", x, y, z);
    }

    fclose(fp);
  }

  if (false)
  {
    FILE * fp = fopen("/opt/mfem_corner_node_ids_2.txt", "w");
    if (!fp)
      fprintf(stderr, "Error: failed to open file.\n");

    for (int node_id : unique_corner_node_ids)
    {
      int ivertex = _mfem_vertex_index_for_libmesh_corner_node_id[node_id];

      double x = vertices[ivertex](0);
      double y = vertices[ivertex](1);
      double z = vertices[ivertex](2);

      fprintf(fp, "(%.2lf, %.2lf, %.2lf)\n", x, y, z);
    }

    fclose(fp);
  }

  // Create the mesh elements.
  buildMFEMElements(num_elements_in_mesh,
                    libmesh_element_type,
                    num_corner_nodes_per_element,
                    unique_block_ids,
                    element_ids_for_block_id,
                    node_ids_for_element_id);
  // libmesh_to_mfem_corner_node_id_map);

  if (false)
  {
    FILE * fp = fopen("/opt/mfem_corner_nodes_element0.txt", "w");

    int ielement = 0;

    mfem::Array<int> vertex_indices;
    GetElementVertices(ielement, vertex_indices);

    for (int i = 0; i < vertex_indices.Size(); i++)
    {
      int ivertex = vertex_indices[i];

      double x = vertices[ivertex](0);
      double y = vertices[ivertex](1);
      double z = vertices[ivertex](2);

      fprintf(fp, "(%.2lf, %.2lf, %.2lf)\n", x, y, z);
    }

    fclose(fp);
  }

  // Create the boundary elements.
  buildMFEMBoundaryElements(libmesh_face_type,
                            num_face_nodes,
                            num_face_corner_nodes,
                            unique_side_boundary_ids,
                            num_elements_for_boundary_id,
                            node_ids_for_boundary_id);
  // libmesh_to_mfem_corner_node_id_map);

  if (false)
  {
    FILE * fp = fopen("/opt/mfem_boundary_nodes.txt", "w");
    if (!fp)
      fprintf(stderr, "Error: failed to open file.\n");

    for (int iboundary = 0; iboundary < NumOfBdrElements; iboundary++)
    {
      mfem::Element * the_element = boundary[iboundary];

      mfem::Array<int> vertex_indices;
      the_element->GetVertices(vertex_indices);

      for (int i = 0; i < vertex_indices.Size(); i++)
      {
        const int ivertex = vertex_indices[i];

        double x = vertices[ivertex](0);
        double y = vertices[ivertex](1);
        double z = vertices[ivertex](2);

        fprintf(fp, "(%.2lf, %.2lf, %.2lf)\n", x, y, z);
      }
    }

    fclose(fp);
  }

  // Handle higher-order meshes.
  const int order = getOrderFromLibmeshElementType(libmesh_element_type);

  if (order == 2)
  {
    handleQuadraticFESpace(libmesh_element_type,
                           unique_block_ids,
                           element_ids_for_block_id,
                           node_ids_for_element_id,
                           coordinates_for_unique_corner_node_id,
                           mfem_dof_for_libmesh_node_id);
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
MFEMMesh::buildMFEMVertices(
    const std::vector<int> & unique_corner_node_ids,
    std::map<int, std::array<double, 3>> & coordinates_for_unique_corner_node_id,
    const int num_dimensions)
{
  _mfem_vertex_index_for_libmesh_corner_node_id.clear();

  NumOfVertices = unique_corner_node_ids.size();
  vertices.SetSize(NumOfVertices);

  // Iterate over the global IDs of each unqiue corner node from the MOOSE mesh.
  const bool use_z_component = (num_dimensions == 3);

  int ivertex = 0;

  // TODO: - are these only corner noddes!!! These must be corner nodes otherwise
  // this will not work.
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

    _mfem_vertex_index_for_libmesh_corner_node_id[global_node_id] = ivertex;

    ivertex++;
  }
}

void
MFEMMesh::buildMFEMElements(const int num_elements_in_mesh,
                            const int libmesh_element_type,
                            const int num_corner_nodes_per_element,
                            const std::vector<int> & unique_block_ids,
                            std::map<int, std::vector<int>> & element_ids_for_block_id,
                            std::map<int, std::vector<int>> & node_ids_for_element_id)
// std::map<int, int> & libmesh_to_mfem_corner_node_id_map)
{
  _libmesh_element_id_for_mfem_element_id.clear();

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
      auto & libmesh_node_ids = node_ids_for_element_id[element_id];

      // Iterate over ONLY the corner nodes in the element.
      for (int ivertex = 0; ivertex < num_corner_nodes_per_element; ivertex++)
      {
        const int libmesh_node_id = libmesh_node_ids[ivertex];

        // Map from the global node ID --> index in the unique_corner_node_ids vector.

        renumbered_vertex_ids[ivertex] =
            _mfem_vertex_index_for_libmesh_corner_node_id[libmesh_node_id];

        // renumbered_vertex_ids[inode] = libmesh_to_mfem_corner_node_id_map[global_node_id];
      }

      _libmesh_element_id_for_mfem_element_id[ielement] = element_id;

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
                                    std::map<int, std::vector<int>> & node_ids_for_boundary_id)
// std::map<int, int> & libmesh_to_mfem_corner_node_id_map)
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
        const int libmesh_node_id = boundary_nodes[jelement * num_face_nodes + knode];

        // Renumber vertex ("node") IDs so they're contiguous and start from 0.
        renumbered_vertex_ids[knode] =
            _mfem_vertex_index_for_libmesh_corner_node_id[libmesh_node_id];
        // libmesh_to_mfem_corner_node_id_map[node_global_index];
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
    std::map<int, std::array<double, 3>> & coordinates_for_unique_corner_node_id,
    std::map<int, int> & mfem_dof_for_libmesh_node_id)
{
  // Clear the map.
  mfem_dof_for_libmesh_node_id.clear();

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

  /*
  Call FinalizeTopology. If we call this then we must call Finalize later after
  we've defined the mesh nodes.
  */
  FinalizeTopology();

  // Define quadratic FE space.
  mfem::FiniteElementCollection * finite_element_collection = new mfem::H1_FECollection(2, 3);

  // NB: note the specified ordering is byVDIM.
  // byVDim: XYZ, XYZ, XYZ, XYZ,...
  // byNode: XXX..., YYY..., ZZZ...
  // TODO: - add checks when we set the components. If we're using the other
  // ordering then we must do this differently otherwise bad things will happen.
  mfem::FiniteElementSpace * finite_element_space =
      new mfem::FiniteElementSpace(this, finite_element_collection, Dim, mfem::Ordering::byVDIM);

  Nodes = new mfem::GridFunction(finite_element_space);
  Nodes->MakeOwner(finite_element_collection); // Nodes will destroy 'finite_element_collection'
  // and 'finite_element_space'
  own_nodes = 1; // NB: - just added this!

  // int ielement = 0;

  std::set<int> libmesh_node_ids_to_print = {13679, 13700, 13800};
  std::set<int> seen_mfem_node_ids;

  struct NodeInfoRec
  {
    union
    {
      int libmesh_element_id;
      int mfem_element_id;
    };

    union
    {
      int libmesh_node_id;
      int mfem_node_id;
    };

    union
    {
      int libmesh_node_index;
      int mfem_node_index;
    };

    union
    {
      int libmesh_block_id;
      int mfem_block_id;
    };
  };

  // Test...
  std::map<int, int> mfem_node_id_for_libmesh_node_id;
  std::map<int, int> libmesh_node_id_for_mfem_node_id;

  // MFEM Node Info --> vector{ Libmesh Node Info }
  std::map<int, std::vector<NodeInfoRec>> shared_mfem_node_map;
  std::map<int, NodeInfoRec> mfem_info_for_id;

  // Loop over blocks.
  if (true)
  {
    for (int ielement = 0; ielement < NumOfElements; ielement++)
    {
      const int libmesh_element_id = _libmesh_element_id_for_mfem_element_id[ielement];

      // for (int block_id : unique_block_ids)
      // {
      //   // Get vector containing all element IDs in block.
      //   auto & element_ids = element_ids_for_block_id[block_id];

      // Loop over element IDs.
      // for (int element_id : element_ids)
      {
        // Get vector containing ALL node global IDs for element.
        auto & libmesh_node_ids = node_ids_for_element_id[libmesh_element_id];

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

          // Map back from the libmesh node ID --> MFEM DOF. This will be required
          // for the higher-order (second-order) mesh transfers. NB: we need to
          // use the dofs array for this. The vdofs array is for xyz components!
          // auto element_node_pair = std::pair<int, int>(libmesh_element_id, libmesh_node_id);

          // mfem_dof_for_libmesh_node_id[element_node_pair] = mfem_node_id;

          // Test: two-way map:
          mfem_node_id_for_libmesh_node_id[libmesh_node_id] = mfem_node_id;
          libmesh_node_id_for_mfem_node_id[mfem_node_id] = libmesh_node_id;

          // Extract node's coordinates:
          auto coordinates = coordinates_for_unique_corner_node_id[libmesh_node_id];

          // Add to testing map:
          NodeInfoRec libmesh_node_info = {.libmesh_element_id = libmesh_element_id,
                                           .libmesh_node_id = libmesh_node_id,
                                           .libmesh_node_index = libmesh_node_index};

          // bool already_seen = false;

          if (seen_mfem_node_ids.count(mfem_node_id))
          {
            // TEST: don't set the coordinates again!
            // already_seen = true;
            shared_mfem_node_map[mfem_node_id].push_back(libmesh_node_info);
          }
          else
          {
            seen_mfem_node_ids.insert(mfem_node_id);

            NodeInfoRec mfem_node_info = {
                .mfem_element_id = ielement, .mfem_node_id = mfem_node_id, .mfem_node_index = j};

            std::vector<NodeInfoRec> new_vector;

            new_vector.push_back(libmesh_node_info);

            shared_mfem_node_map[mfem_node_info.mfem_node_id] = new_vector;
            mfem_info_for_id[mfem_node_info.mfem_node_id] = mfem_node_info;
          }

          // TODO: - eventually add sanity checks to make sure that the mfem node ID
          // doesn't map back to a different libmesh node ID on a different element!

          // Compare the coordinates we've just set with what we get when we call GetNode():
          double coordinates_expected[3] = {coordinates[0], coordinates[1], coordinates[2]};

          // Set xyz components using the VDIM ordering:
          // NB: vdofs using xxx, yyy, zzz ordering now...
          // if (already_seen)
          //   continue; // Test --> not setting coordinates.

          (*Nodes)(vdofs[j]) = coordinates[0];
          (*Nodes)(vdofs[j + dofs.Size()]) = coordinates[1];

          if (Dim == 3)
          {
            (*Nodes)(vdofs[j + 2 * dofs.Size()]) = coordinates[2];
          }
        }

        // ielement++;
      }
    }
  }

  // Test way of doing things:
  // Do all faces first, then do the central node.
  // if (false)
  // {
  //   for (int ielement = 0; ielement < NumOfElements; ielement++)
  //   {
  //     auto libmesh_element_id = _libmesh_element_id_for_mfem_element_id[ielement];

  //     mfem::Array<int> faces;
  //     mfem::Array<int> face_orientations;

  //     GetElementFaces(ielement, faces, face_orientations);

  //     for (int iface = 0; iface < faces.Size(); iface++)
  //     {
  //       mfem::Array<int> face_dofs;
  //       finite_element_space->GetFaceDofs(faces[iface], face_dofs);

  //       mfem::Array<int> face_vdofs;
  //       finite_element_space->GetFaceVDofs(faces[iface], face_vdofs);

  //       // Set nodes to the correct one which should be the coordinates of the
  //       // last node in the libmesh node ids for an element.
  //       auto & libmesh_face_node_ids =
  //           _libmesh_global_face_node_ids_for_element_id[ielement][iface];

  //       for (int j = 0; j < face_dofs.Size(); j++)
  //       {
  //         const int mfem_node_dof = face_dofs[j];

  //         // Correct the central node id:
  //         libmesh_node_id_for_mfem_node_id[hex27_central_node_dof] = libmesh_central_node_id;
  //         mfem_node_id_for_libmesh_node_id[libmesh_central_node_id] = hex27_central_node_dof;

  //         auto coordinates_for_libmesh_central_node_id =
  //             coordinates_for_unique_corner_node_id[libmesh_central_node_id];

  //         double x = coordinates_for_libmesh_central_node_id[0];
  //         double y = coordinates_for_libmesh_central_node_id[1];
  //         double z = coordinates_for_libmesh_central_node_id[2];

  //         (*Nodes)(face_vdofs[face_dofs.Size() - 1]) = x;
  //         (*Nodes)(face_vdofs[face_dofs.Size() - 1 + face_dofs.Size()]) = y;
  //         (*Nodes)(face_vdofs[face_dofs.Size() - 1 + 2 * face_dofs.Size()]) = z;
  //       }
  //     }
  //   }
  // }

  // Notes:
  // edges -- correct
  // vertices (lower-order corner nodes) -- correct
  // middle face node --> incorrect

  // Test code:
  // The middle node from each face is incorrect. Attempt to remedy this:
  bool fix_bollocked_central_face_node = false;

  if (fix_bollocked_central_face_node)
  {
    int printer_credit_remaining = 10;

    for (int ielement = 0; ielement < NumOfElements; ielement++)
    {
      auto libmesh_element_id = _libmesh_element_id_for_mfem_element_id[ielement];

      mfem::Array<int> faces;
      mfem::Array<int> face_orientations;

      GetElementFaces(ielement, faces, face_orientations);

      for (int iface = 0; iface < faces.Size(); iface++)
      {
        mfem::Array<int> face_dofs;
        finite_element_space->GetFaceDofs(faces[iface], face_dofs);

        // Only interested in last dof for each face --> central node (hex27).
        const int hex27_central_node_dof = face_dofs.Last();

        mfem::Array<int> face_vdofs;
        finite_element_space->GetFaceVDofs(faces[iface], face_vdofs);

        // Set nodes to the correct one which should be the coordinates of the
        // last node in the libmesh node ids for an element.
        auto & libmesh_face_node_ids =
            _libmesh_global_face_node_ids_for_element_id[ielement][iface];

        auto libmesh_central_node_id = libmesh_face_node_ids.back();

        // Correct the central node id:
        libmesh_node_id_for_mfem_node_id[hex27_central_node_dof] = libmesh_central_node_id;
        mfem_node_id_for_libmesh_node_id[libmesh_central_node_id] = hex27_central_node_dof;

        // auto pair = std::pair<int, int>(libmesh_element_id, libmesh_central_node_id);

        // mfem_dof_for_libmesh_node_id[pair] = hex27_central_node_dof;

        if (--printer_credit_remaining > 0)
        {
          printf(
              "MFEM node %d (element: %d), now being remapped to libmesh node %d (element: %d)\n",
              hex27_central_node_dof,
              ielement,
              libmesh_central_node_id,
              libmesh_element_id);
        }

        auto coordinates_for_libmesh_central_node_id =
            coordinates_for_unique_corner_node_id[libmesh_central_node_id];

        double x = coordinates_for_libmesh_central_node_id[0];
        double y = coordinates_for_libmesh_central_node_id[1];
        double z = coordinates_for_libmesh_central_node_id[2];

        (*Nodes)(face_vdofs[face_dofs.Size() - 1]) = x;
        (*Nodes)(face_vdofs[face_dofs.Size() - 1 + face_dofs.Size()]) = y;
        (*Nodes)(face_vdofs[face_dofs.Size() - 1 + 2 * face_dofs.Size()]) = z;
      }
    }
  }

  // The only node that is now incorrect is the central node of the hex27 element.
  // For libmesh, this will be the last node in each element.
  bool fix_bollocked_central_hex27_node = false;

  if (fix_bollocked_central_hex27_node)
  {
    printf("Yes, I'm fixing the bollocked central hex27 node...\n");

    for (int ielement = 0; ielement < NumOfElements; ielement++)
    {
      mfem::Array<int> interior_dofs;
      finite_element_space->GetElementInteriorDofs(ielement, interior_dofs);

      // printf("interior_dof.Size() = %d, %d\n", interior_dofs.Size(), interior_dofs[0]);

      mfem::Array<int> interior_vdofs;
      finite_element_space->GetElementInteriorVDofs(ielement, interior_vdofs);

      auto libmesh_element_id = _libmesh_element_id_for_mfem_element_id[ielement];
      auto & libmesh_node_ids = node_ids_for_element_id[libmesh_element_id];

      const int last_libmesh_node_id = libmesh_node_ids.back();

      auto & coordinates = coordinates_for_unique_corner_node_id[last_libmesh_node_id];

      double x = coordinates[0], y = coordinates[1], z = coordinates[2];

      (*Nodes)(interior_vdofs[0]) = x;
      (*Nodes)(interior_vdofs[0 + interior_dofs.Size()]) = y;
      (*Nodes)(interior_vdofs[0 + 2 * interior_dofs.Size()]) = z;

      // Correct the node ids:
      libmesh_node_id_for_mfem_node_id[interior_dofs[0]] = last_libmesh_node_id;
      mfem_node_id_for_libmesh_node_id[last_libmesh_node_id] = interior_dofs[0];
    }
  }

  /**
   * Set the map:
   */
  mfem_dof_for_libmesh_node_id = mfem_node_id_for_libmesh_node_id;

  /**
   * Test: print the mfem node dofs.
   */
  if (false)
  {
    for (int ielement = 0; ielement < NumOfElements; ielement++)
    {
      mfem::Array<int> all_dofs;
      finite_element_space->GetElementDofs(ielement, all_dofs);

      std::set<int> unloved_dofs;
      for (int dof : all_dofs)
      {
        unloved_dofs.insert(dof);
      }

      mfem::Array<int> interior_dofs;
      finite_element_space->GetElementInteriorDofs(ielement, interior_dofs);
      for (int interior_dof : interior_dofs)
      {
        unloved_dofs.erase(interior_dof);
      }

      mfem::Array<int> faces;
      mfem::Array<int> faces_orientation;

      GetElementFaces(ielement, faces, faces_orientation);

      for (int face : faces)
      {
        mfem::Array<int> face_dofs;
        finite_element_space->GetFaceDofs(face, face_dofs);

        for (int face_dof : face_dofs)
        {
          unloved_dofs.erase(face_dof);
        }
      }

      if (unloved_dofs.size() == 0)
      {
        continue;
      }

      printf("Element %d: ", ielement);

      for (int unloved_dof : unloved_dofs)
      {
        printf("%d, ", unloved_dof);
      }

      printf("\n");
    }
  }

  /**
   * Test: print mfem edges of each element.
   */
  if (false)
  {
    FILE * fp = fopen("/opt/mfem_edges.txt", "w");
    if (!fp)
    {
      fprintf(stderr, "Error: cannot open file!\n");
    }

    for (int ielement = 0; ielement < NumOfElements; ielement++)
    {
      fprintf(fp, "Element %d:\n", ielement);

      mfem::Array<int> edges;
      mfem::Array<int> edge_orientations; // ???

      GetElementEdges(ielement, edges, edge_orientations);

      for (int iedge = 0; iedge < edges.Size(); iedge++)
      {
        fprintf(fp, "Edge %d:\n", iedge);

        mfem::Array<int> edge_dofs;
        finite_element_space->GetEdgeDofs(edges[iedge], edge_dofs);

        for (int i = 0; i < edge_dofs.Size(); i++)
        {
          double coordinates[3];
          GetNode(edge_dofs[i], coordinates);

          double x = coordinates[0], y = coordinates[1], z = coordinates[2];
          fprintf(fp, "(%.2lf, %.2lf, %.2lf)\n", x, y, z);
        }
      }
    }

    fclose(fp);
  }

  /**
   * Test: print faces of elements.
   */
  if (true)
  {
    std::set<int> interesting_mfem_elements_to_print = {0, 1, 2, 3, 4, 76};

    FILE * fp = fopen("/opt/mfem_faces.txt", "w");
    if (!fp)
    {
      fprintf(stderr, "Error: cannot open file!\n");
    }

    for (int ielement = 0; ielement < NumOfElements; ielement++)
    // for (int ielement : interesting_mfem_elements_to_print)
    {
      auto libmesh_element_id = _libmesh_element_id_for_mfem_element_id[ielement];
      auto & libmesh_node_ids = node_ids_for_element_id[libmesh_element_id];

      fprintf(fp, "Element %d:\n", ielement);

      mfem::Array<int> faces;
      mfem::Array<int> face_orientations;

      GetElementFaces(ielement, faces, face_orientations);

      // Get all of the element's dofs:
      mfem::Array<int> element_dofs;
      finite_element_space->GetElementDofs(ielement, element_dofs);

      std::map<int, int> index_for_element_dof;

      for (int i = 0; i < element_dofs.Size(); i++)
      {
        index_for_element_dof[element_dofs[i]] = i;
      }

      for (int iface = 0; iface < faces.Size(); iface++)
      {
        fprintf(fp, "Face %d:\n", iface);

        mfem::Array<int> face_dofs;
        finite_element_space->GetFaceDofs(faces[iface], face_dofs);

        for (int i = 0; i < face_dofs.Size(); i++)
        {
          const int face_dof = face_dofs[i];

          double coordinates[3];
          GetNode(face_dof, coordinates);

          // What's the index of the face_dof? Use this to determine the
          // corresponding libmesh node id.
          const int index = index_for_element_dof[face_dof];

          const int libmesh_node_index = mfem_to_libmesh_map[index] - 1;

          const int libmesh_node_id = libmesh_node_ids[libmesh_node_index];
          auto & libmesh_coordinates = coordinates_for_unique_corner_node_id[libmesh_node_id];

          double x = coordinates[0], y = coordinates[1], z = coordinates[2];

          // fprintf(fp, "(%.2lf, %.2lf, %.2lf)\n", x, y, z);
          fprintf(fp, "(%.2lf, %.2lf, %.2lf)\n", x, y, z);
        }
      }
    }

    fclose(fp);
  }

  /**
   * Test: print nodes of mfem elements.
   */
  if (true)
  {
    std::set<int> element_ids_to_print = {0, 1, 2, 3, 4, 76};

    FILE * fp = fopen("/opt/mfem_elements.txt", "w");
    if (!fp)
    {
      fprintf(stderr, "Error: could not open file.\n");
    }

    for (int ielement : element_ids_to_print)
    {
      fprintf(fp, "MFEM Element %4d:\n", ielement);

      mfem::Array<int> mfem_node_ids;
      finite_element_space->GetElementDofs(ielement, mfem_node_ids);

      double coordinates_returned[3];

      for (int mfem_node_id : mfem_node_ids)
      {
        GetNode(mfem_node_id, coordinates_returned);

        fprintf(fp,
                "(%.2lf, %.2lf, %.2lf)\n",
                coordinates_returned[0],
                coordinates_returned[1],
                coordinates_returned[2]);
      }

      fprintf(fp, "\n");
    }

    fclose(fp);
  }

  /**
   * Test: print nodes of libmesh elements.
   */
  if (true)
  {
    FILE * fp = fopen("/opt/libmesh_elements.txt", "w");
    if (!fp)
    {
      fprintf(stderr, "Error: could not open file.\n");
    }

    for (int block_id : unique_block_ids)
    {
      for (int element_id : element_ids_for_block_id[block_id])
      {
        fprintf(fp, "Element %d:\n", element_id);

        auto & node_ids = node_ids_for_element_id[element_id];

        for (int node_id : node_ids)
        {
          auto & coordinates = coordinates_for_unique_corner_node_id[node_id];

          fprintf(fp,
                  "(%.2lf, %.2lf, %.2lf), libmesh_node_id=%d\n",
                  coordinates[0],
                  coordinates[1],
                  coordinates[2],
                  node_id);
        }
      }
    }

    fclose(fp);
  }

  // Test: new comparison.
  if (true)
  {
    // Create a set of all unique libmesh node ids.
    std::set<int> libmesh_node_ids;

    for (int block_id : unique_block_ids)
    {
      for (int libmesh_element : element_ids_for_block_id[block_id])
      {
        auto & libmesh_node_ids_for_element = node_ids_for_element_id[libmesh_element];

        for (int libmesh_node_id : libmesh_node_ids_for_element)
        {
          libmesh_node_ids.insert(libmesh_node_id);
        }
      }
    }

    for (int ielement = 0; ielement < NumOfElements; ielement++)
    {
      mfem::Array<int> mfem_dofs;
      finite_element_space->GetElementDofs(ielement, mfem_dofs);

      for (int mfem_dof : mfem_dofs)
      {
        double mfem_coordinates[3];
        GetNode(mfem_dof, mfem_coordinates);

        const int libmesh_node_id = libmesh_node_id_for_mfem_node_id[mfem_dof];
        auto & coordinates = coordinates_for_unique_corner_node_id[libmesh_node_id];

        // Remove from set.
        libmesh_node_ids.erase(libmesh_node_id);

        double libmesh_coordinates[3] = {coordinates[0], coordinates[1], coordinates[2]};

        if (!CoordinatesMatch(libmesh_coordinates, mfem_coordinates))
        {
          printf("libmesh_node_id = %d, (%.2lf, %.2lf, %.2lf) != "
                 "mfem_node_id = %d, (%.2lf, %.2lf, "
                 "%.2lf)\n",
                 libmesh_node_id,
                 libmesh_coordinates[0],
                 libmesh_coordinates[1],
                 libmesh_coordinates[2],
                 mfem_dof,
                 mfem_coordinates[0],
                 mfem_coordinates[1],
                 mfem_coordinates[2]);
        }
      }
    }

    // Check how many elements remain in set of libmesh element ids. Ideally,
    // there should be none left indicating that we've referenced every single
    // element in the set.
    printf("There are %d elements left in the libmesh nodes set.\n", libmesh_node_ids.size());

    for (int node_id : libmesh_node_ids)
    {
      printf("Unrefereneced libmesh node %d\n", node_id);
    }
  }
}

static bool
CoordinatesMatch(double primary[3], double secondary[3])
{
  for (int i = 0; i < 3; i++)
  {
    if (fabs(primary[i] - secondary[i]) > 0.01)
    {
      return false;
    }
  }

  return true;
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
