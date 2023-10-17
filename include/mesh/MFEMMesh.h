#pragma once

#include "libmesh/elem.h"
#include "libmesh/enum_io_package.h"
#include "libmesh/exodusII_io.h"
#include "libmesh/nemesis_io.h"
#include "libmesh/node.h"
#include "libmesh/parallel_mesh.h"
#include "mfem.hpp"

/**
 * MFEMMesh
 *
 * MFEMMesh wraps an mfem::Mesh object.
 */
class MFEMMesh : public mfem::Mesh
{
public:
  MFEMMesh(const int num_dimensions,
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
           std::map<int, std::vector<std::vector<int>>> & libmesh_face_node_ids_for_element_id,
           std::map<int, std::vector<int>> & element_ids_for_block_id,
           std::map<int, std::vector<int>> & node_ids_for_element_id,
           std::map<int, std::vector<int>> & node_ids_for_boundary_id,
           std::map<int, std::array<double, 3>> & coordinates_for_unique_corner_node_id);

  MFEMMesh(std::string mesh_fname,
           int generate_edges = 0,
           int refine = 1,
           bool fix_orientation = true);

  // TODO: - add safeties if not second order.
  inline std::map<int, int> & getLibmeshNodeIDForMFEMNodeIDMap()
  {
    return _libmesh_node_id_for_mfem_node_id;
  }

  inline std::map<int, int> & getMFEMNodeIDForLibmeshNodeIDMap()
  {
    return _mfem_node_id_for_libmesh_node_id;
  }

protected:
  /**
   * Sets the protected variable array using the provided vector of corner node
   * IDs from MOOSE. Note that the vertices (named "nodes" in MOOSE) are ONLY
   * at the corners of elements. These are referred to as "corner nodes" in MOOSE.
   */
  void
  buildMFEMVertices(const std::vector<int> & unique_corner_node_ids,
                    std::map<int, std::array<double, 3>> & coordinates_for_unique_corner_node_id,
                    const int num_dimensions);

  /**
   * Construct the MFEM elements array.
   */
  void buildMFEMElements(const int num_elements_in_mesh,
                         const int libmesh_element_type,
                         const int num_corner_nodes_per_element,
                         const std::vector<int> & unique_block_ids,
                         std::map<int, std::vector<int>> & element_ids_for_block_id,
                         std::map<int, std::vector<int>> & node_ids_for_element_id);

  /**
   * Construct the boundary array of elements.
   */
  void buildMFEMBoundaryElements(const int libmesh_face_type,
                                 const int num_face_nodes,
                                 const int num_face_corner_nodes,
                                 const std::vector<int> & unique_side_boundary_ids,
                                 std::map<int, int> & num_elements_for_boundary_id,
                                 std::map<int, std::vector<int>> & node_ids_for_boundary_id);

  /**
   * Returns a pointer to an mfem::Element.
   */
  mfem::Element *
  buildMFEMElement(const int element_type, const int * vertex_ids, const int block_id);

  /**
   * Returns an pointer to an mfem::Element (for faces only).
   */
  mfem::Element *
  buildMFEMFaceElement(const int face_type, const int * vertex_ids, const int boundary_id);

  /**
   * Called internally in constructor if the order == 2.
   */
  void handleQuadraticFESpace(
      const int libmesh_element_type,
      const std::vector<int> & unique_block_ids,
      std::map<int, std::vector<int>> & element_ids_for_block_id,
      std::map<int, std::vector<int>> & node_ids_for_element_id,
      std::map<int, std::array<double, 3>> & coordinates_for_unique_corner_node_id,
      std::map<int, std::vector<std::vector<int>>> & libmesh_face_node_ids_for_element_id);
  /**
   * Fixes the node ordering for hex27 second-order mesh elements.
   */
  void fixHex27MeshNodes(
      mfem::FiniteElementSpace & finite_element_space,
      std::map<int, std::vector<std::vector<int>>> & libmesh_face_node_ids_for_element_id,
      std::map<int, std::array<double, 3>> & coordinates_for_unique_corner_node_id,
      std::map<int, std::vector<int>> & node_ids_for_element_id);

  /**
   * Returns a set containing all libmesh node ids.
   */
  std::set<int>
  buildSetContainingLibmeshNodeIDs(const std::vector<int> & unique_block_ids,
                                   std::map<int, std::vector<int>> & element_ids_for_block_id,
                                   std::map<int, std::vector<int>> & node_ids_for_element_id);

  /**
   * Verifies whether the libmesh and mfem node ids have a unique mapping. All
   * coordinates should match and every mfem node id should have a corresponding
   * libmesh node id. Any left-over node ids will be detected.
   */
  void verifyUniqueMappingBetweenLibmeshAndMFEMNodeIDs(
      mfem::FiniteElementSpace & finite_element_space,
      const std::vector<int> & unique_block_ids,
      std::map<int, std::vector<int>> & element_ids_for_block_id,
      std::map<int, std::vector<int>> & node_ids_for_element_id,
      std::map<int, std::array<double, 3>> & coordinates_for_libmesh_node_id);

  /**
   * Prints debugging info to specified files for second-order mesh elements. Pass
   * a NULL argument for any paths to skip.
   */
  void writeSecondOrderElementInfoToFiles(const char * fpathNodes,
                                          const char * fpathEdges,
                                          const char * fpathFaces,
                                          mfem::FiniteElementSpace & finite_element_space);

  /**
   * Determines the order from the libmesh element type provided.
   */
  const int getOrderFromLibmeshElementType(int libmesh_element_type) const;

  enum CubitFaceType
  {
    FACE_EDGE2,
    FACE_EDGE3,
    FACE_TRI3,
    FACE_TRI6,
    FACE_QUAD4,
    FACE_QUAD9
  };

  enum CubitElementType
  {
    ELEMENT_TRI3,
    ELEMENT_TRI6,
    ELEMENT_QUAD4,
    ELEMENT_QUAD9,
    ELEMENT_TET4,
    ELEMENT_TET10,
    ELEMENT_HEX8,
    ELEMENT_HEX27
  };

  // TODO: - add descriptions.
  std::map<int, int> _libmesh_node_id_for_mfem_node_id;
  std::map<int, int> _mfem_node_id_for_libmesh_node_id;

  std::map<int, int> _libmesh_element_id_for_mfem_element_id;
  std::map<int, int> _mfem_vertex_index_for_libmesh_corner_node_id;
};
