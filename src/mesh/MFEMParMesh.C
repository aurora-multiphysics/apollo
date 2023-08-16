#pragma once
#include "MFEMParMesh.h"

// NB: This is a temporary initializer and will be replaced once this class
// has been completed.
MFEMParMesh::MFEMParMesh(MPI_Comm comm, MFEMMesh & mesh, int * partitioning, int part_method)
  : mfem::ParMesh(comm, dynamic_cast<mfem::Mesh &>(mesh), partitioning, part_method)
{
}

// #include "mpi.h"
// #include "/opt/moose/framework/include/mesh/MooseMesh.h"
// #include "/opt/moose/libmesh/include/mesh/distributed_mesh.h"

// Objective is to create a distributed mesh from an existing distributed moose
// mesh. We won't have access to all of the mesh. The processor iDs etc and nodes
// should be created as for the moose_mesh.
// MFEMParMesh::MFEMParMesh(MPI_Comm comm, MooseMesh &moose_mesh)
// {
//   // Get the underlying libmesh representation and convert from the base class
//   // MeshBase to DistributedMesh.
//   libmesh::MeshBase &libmesh_base = moose_mesh.getMesh();

//   MyComm = comm;
//   MPI_Comm_size(MyComm, &NRanks);   // How many processors are there?
//   MPI_Comm_rank(MyComm, &MyRank);   // What is the processor ID?

//   // TODO: - this is not necessarily correct is it?!? Dim == spaceDim??
//   Dim = spaceDim = moose_mesh.dimension();

//   // We only want to iterate over local nodes (that are on same processor!). Run
//   // through these to generate coordinate data structure.
//   mfem::vector<double> coordinates_x(libmesh_base.n_local_nodes(), 0);
//   mfem::vector<double> coordinates_y(libmesh_base.n_local_nodes(), 0);
//   mfem::vector<double> coordinates_z(libmesh_base.n_local_nodes(), 0);

//   for (auto node : libmesh_base.local_node_ptr_range())
//   {
//     unsigned int node_id = node->id();

//     coordinates_x[node_id] = (*node)(0);
//     coordinates_y[node_id] = (*node)(1);
//     coordinates_z[node_id] = (*node)(2);
//   }

//   const size_t num_local_sidesets = 0;

//   unsigned short num_blocks_in_mesh_on_processor = libmesh_base.n_local_subdomains();

//   auto num_elements_per_block = std::make_unique<size_t>(num_blocks_in_mesh_on_processor);

//   for (int iblock : )

// }

// // TODO: - simplify. We should be using C++ stuff here if possible...
// MFEMParMesh::MFEMParMesh(const int num_dimensions,
//                          const int num_elements,
//                          const int num_vertices,
//                          const int num_nodes_per_element,
//                          const int num_edge_nodes_per_element,
//                          const int num_element_blocks,
//                          const int libmesh_element_type,
//                          const int *num_elements_in_block,
//                          const int *processor_id_of_each_element,   // aka partitioning
//                          const int *vertex_coordinates_x, // TODO: - temporary --> switch to
//                          vector. const int *vertex_coordinates_y, const int
//                          *vertex_coordinates_z, const int *vertex_unique_ids_1_indexed, int
//                          **node_ids_in_block,      // [block_id][nodes_of_all_elements_in_block],
//                          NB: called elem_blk in CoupledMFEMMesh int *block_attributes,       //
//                          Attributes for each block. Named ebprop in CoupledMFEMMesh std::map<int,
//                          int> &cubit_to_mfem_vertex_map)
// {
//   // TODO: - add helpful safety checks here...

//   MyComm = MPI_COMM_WORLD;
//   MPI_Comm_size(MyComm, &NRanks);
//   MPI_Comm_rank(MyComm, &MyRank);

//   // TODO: - assume non-conforming.

//   Dim = spaceDim = num_dimensions;

//   // TODO: - setup number of vertices and number of elements.
//   NumOfElements = 0;  // TODO: - set.

//   // Create the array of vertices for all processors:
//   const int total_num_vertices = num_vertices;
//   mfem::Array<mfem::Vertex>all_vertices(total_num_vertices);

//   // TODO: - separate this out into its own function with better documentation.
//   for (int ivertex = 0; ivertex < total_num_vertices; ivertex++)
//   {
//     const int unique_id = vertex_unique_ids_1_indexed[ivertex];

//     all_vertices[ivertex](0) = vertex_coordinates_x[unique_id - 1];
//     all_vertices[ivertex](1) = vertex_coordinates_y[unique_id - 1];

//     if (Dim == 3 && vertex_coordinates_z)
//     {
//       all_vertices[ivertex](2) = vertex_coordinates_z[unique_id - 1];
//     }
//   }

//   // Create the array of elements for all processors:
//   mfem::Array<mfem::Element *>all_elements(num_elements);

//   int element_counter = 0;

//   for (int iblock = 0; iblock < num_element_blocks; iblock++)
//   {
//     int *node_ids_of_all_elements_in_block = node_ids_in_block[iblock];

//     int block_attribute = block_attributes[iblock];

//     for (int jelement = 0; jelement < num_elements_in_block[iblock]; jelement++)
//     {
//       // Pointer arithmetic to set start point along 1D array of integers:
//       int *node_ids_of_jelement = (node_ids_of_all_elements_in_block + jelement *
//       num_nodes_per_element);

//       int renumbered_vertex_ids[num_edge_nodes_per_element];

//       // Run through only the nodes on the edge of the element to get renumbered
//       // vertex ids. This allows us to create Element objects.
//       for (int knode = 0; knode < num_edge_nodes_per_element; knode++)
//       {
//         const int node_id = node_ids_of_jelement[knode];

//         renumbered_vertex_ids[knode] = cubit_to_mfem_vertex_map[node_id + 1]; // TODO: - why is
//         it one-indexed. Think about.
//       }

//       switch (libmesh_element_type)
//       {
//         case ELEMENT_TRI3:
//         case ELEMENT_TRI6:
//         {
//           all_elements[element_counter] = new mfem::Triangle(renumbered_vertex_ids,
//           block_attribute); break;
//         }
//         case ELEMENT_QUAD4:
//         case ELEMENT_QUAD9:
//         {
//           all_elements[element_counter] = new mfem::Quadrilateral(renumbered_vertex_ids,
//           block_attribute); break;
//         }
//         case ELEMENT_TET4:
//         case ELEMENT_TET10:
//         {
//           #ifdef MFEM_USE_MEMALLOC
//           all_elements[element_counter] = TetMemory.Alloc();
//           all_elements[element_counter]->SetVertices(renumbered_vertex_ids);
//           all_elements[element_counter]->SetAttribute(block_attribute);
//           #else
//           all_elements[element_counter] = new mfem::Tetrahedron(renumbered_vertex_ids,
//           block_attribute); #endif break;
//         }
//       case ELEMENT_HEX8:
//       case ELEMENT_HEX27:
//       {
//         all_elements[element_counter] = new mfem::Hexahedron(renumbered_vertex_ids,
//         block_attribute); break;
//       }
//       default:
//       {
//         MFEM_ABORT("Invalid libmesh_element_type specified.");
//         break;
//       }
//     }

//     element_counter++;
//   }

//   // From all the elements, we need to build the local vertices.
//   for (int ielement = 0; ielement < num_elements; ielement++)
//   {
//     if (processor_id_of_each_element[ielement] == MyRank)
//     {
//       mfem::Element *the_element = all_elements[ielement];

//       mfem::Array<int>element_vertices;
//       the_element->GetVertices(&element_vertices);

//       for (int jvertex = 0; jvertex < element_vertices.Size(); jvertex++)
//       {

//       }

//     }
//   }

//   // TODO: - once the elements have been created then we can call our version of BuildLocalVertices similar to pmesh
//   // TODO: - once we have a better understanding of what we're doing, we can tidy, simplify and splitup
//   // TODO: - once it's working rewrite CoupledMFEMMesh entirely and MFEMMesh's conversion BS code.

//   // Now we need to build the local vertices:
//   NumOfVertices = BuildLocalVertices(processor_id_of_each_element);

// }

// int MFEMParMesh::BuildLocalVertices(const int *partitioning)
// {

// }
