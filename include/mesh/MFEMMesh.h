#pragma once

#include "libmesh/elem.h"
#include "libmesh/enum_io_package.h"
#include "libmesh/exodusII_io.h"
#include "libmesh/nemesis_io.h"
#include "libmesh/node.h"
#include "libmesh/parallel_mesh.h"
#include "CubitElementInfo.h"
#include "mfem.hpp"

/**
 * MFEMMesh
 *
 * MFEMMesh wraps an mfem::Mesh object.
 */
class MFEMMesh : public mfem::Mesh
{
public:
  MFEMMesh(const int num_elements_in_mesh,
           const CubitElementInfo & element_info,
           const std::vector<int> & unique_block_ids,
           const std::vector<int> & unique_side_boundary_ids,
           const std::vector<int> & unique_libmesh_corner_node_ids,
           std::map<int, int> & num_elements_for_boundary_id,
           std::map<int, std::vector<int>> & libmesh_element_ids_for_block_id,
           std::map<int, std::vector<int>> & libmesh_node_ids_for_element_id,
           std::map<int, std::vector<int>> & libmesh_node_ids_for_boundary_id,
           std::map<int, std::array<double, 3>> & coordinates_for_libmesh_node_id,
           std::map<int, std::vector<int>> * libmesh_center_of_face_node_ids_for_hex27_element_id =
               nullptr);

  MFEMMesh(std::string mesh_fname,
           int generate_edges = 0,
           int refine = 1,
           bool fix_orientation = true);

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
  void buildMFEMVertices(const std::vector<int> & unique_libmesh_corner_node_ids,
                         std::map<int, std::array<double, 3>> & coordinates_for_libmesh_node_id);

  /**
   * Construct the MFEM elements array.
   */
  void buildMFEMElements(const int num_elements_in_mesh,
                         const CubitElementInfo & element_info,
                         const std::vector<int> & unique_block_ids,
                         std::map<int, std::vector<int>> & libmesh_element_ids_for_block_id,
                         std::map<int, std::vector<int>> & libmesh_node_ids_for_element_id);

  /**
   * Construct the boundary array of elements.
   */
  void
  buildMFEMBoundaryElements(const CubitElementInfo & element_info,
                            const std::vector<int> & unique_side_boundary_ids,
                            std::map<int, int> & num_elements_for_boundary_id,
                            std::map<int, std::vector<int>> & libmesh_node_ids_for_boundary_id);

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
   * Called internally in constructor if the element is second-order.
   */
  void handleQuadraticFESpace(
      const CubitElementInfo & element_info,
      const std::vector<int> & unique_block_ids,
      std::map<int, std::vector<int>> & libmesh_element_ids_for_block_id,
      std::map<int, std::vector<int>> & libmesh_node_ids_for_element_id,
      std::map<int, std::array<double, 3>> & coordinates_for_libmesh_node_id,
      std::map<int, std::vector<int>> * libmesh_center_of_face_node_ids_for_hex27_element_id);

  /**
   * Fixes the node ordering for hex27 second-order mesh elements. This is called
   * internally in handleQuadraticFESpace. This method should be called only after
   * the Nodes have been created and their coordinates have been set.
   */
  void fixHex27MeshNodes(
      mfem::FiniteElementSpace & finite_element_space,
      std::map<int, std::array<double, 3>> & coordinates_for_libmesh_node_id,
      std::map<int, std::vector<int>> & libmesh_node_ids_for_element_id,
      std::map<int, std::vector<int>> & libmesh_center_of_face_node_ids_for_hex27_element_id);

  /**
   * Verifies whether the libmesh and mfem node ids have a unique mapping. All
   * coordinates should match and every mfem node id should have a corresponding
   * libmesh node id. Any left-over node ids will be detected.
   */
  void verifyUniqueMappingBetweenLibmeshAndMFEMNodes(
      mfem::FiniteElementSpace & finite_element_space,
      const std::vector<int> & unique_block_ids,
      std::map<int, std::vector<int>> & libmesh_element_ids_for_block_id,
      std::map<int, std::vector<int>> & libmesh_node_ids_for_element_id,
      std::map<int, std::array<double, 3>> & coordinates_for_libmesh_node_id);

  /**
   * Writes debugging info to specified files for second-order mesh elements. No
   * information will be printed if a NULL path is supplied.
   */
  void writeSecondOrderElementDebuggingInfo(const char * fpathNodes,
                                            const char * fpathEdges,
                                            const char * fpathFaces,
                                            mfem::FiniteElementSpace & finite_element_space);
  /**
   * A two-way map between libmesh and mfem node ids. This is only used for
   * higher-order transfers.
   */
  std::map<int, int> _libmesh_node_id_for_mfem_node_id;
  std::map<int, int> _mfem_node_id_for_libmesh_node_id;

  /**
   * Map from the MFEM element id to the libmesh element id internally.
   */
  std::map<int, int> _libmesh_element_id_for_mfem_element_id;

  /**
   * Maps from the libmesh corner node id to the mfem vertex id internally.
   */
  std::map<int, int> _mfem_vertex_index_for_libmesh_corner_node_id;
};
