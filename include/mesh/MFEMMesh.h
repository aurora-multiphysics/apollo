#pragma once

#include "libmesh/elem.h"
#include "libmesh/enum_io_package.h"
#include "libmesh/exodusII_io.h"
#include "libmesh/nemesis_io.h"
#include "libmesh/node.h"
#include "libmesh/parallel_mesh.h"
#include "CubitElementInfo.h"
#include "NodeBiMap.h"
#include "mfem.hpp"

/**
 * MFEMMesh
 *
 * MFEMMesh wraps an mfem::Mesh object.
 */
class MFEMMesh : public mfem::Mesh
{
public:
  /**
   * Initializer for 1st order elements.
   */
  MFEMMesh(const int num_elements_in_mesh,
           const CubitElementInfo & element_info,
           const std::vector<int> & unique_block_ids,
           const std::vector<int> & unique_side_boundary_ids,
           const std::vector<int> & unique_libmesh_corner_node_ids,
           std::map<int, int> & num_elements_for_boundary_id,
           std::map<int, std::vector<int>> & libmesh_element_ids_for_block_id,
           std::map<int, std::vector<int>> & libmesh_node_ids_for_element_id,
           std::map<int, std::vector<int>> & libmesh_node_ids_for_boundary_id,
           std::map<int, std::array<double, 3>> & coordinates_for_libmesh_node_id);

  /**
   * Initializer for 2nd order elements.
   */
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
           std::map<int, std::array<int, 6>> & libmesh_center_of_face_node_ids_for_hex27_element_id,
           NodeBiMap & second_order_node_bimap);

  MFEMMesh(std::string mesh_fname,
           int generate_edges = 0,
           int refine = 1,
           bool fix_orientation = true);

protected:
  /**
   * Calls buildMFEMVertices, buildMFEMElements, buildMFEMBoundaryElements methods
   * to construct the mesh. NB: - additional methods should be called after this
   * to handle second-order elements. The Finalize() method must be called at the
   * end.
   */
  void buildMFEMVerticesAndElements(
      const int num_elements_in_mesh,
      const CubitElementInfo & element_info,
      const std::vector<int> & unique_block_ids,
      const std::vector<int> & unique_side_boundary_ids,
      const std::vector<int> & unique_libmesh_corner_node_ids,
      std::map<int, int> & num_elements_for_boundary_id,
      std::map<int, std::vector<int>> & libmesh_element_ids_for_block_id,
      std::map<int, std::vector<int>> & libmesh_node_ids_for_element_id,
      std::map<int, std::vector<int>> & libmesh_node_ids_for_boundary_id,
      std::map<int, std::array<double, 3>> & coordinates_for_libmesh_node_id);

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
      std::map<int, std::array<int, 6>> & libmesh_center_of_face_node_ids_for_hex27_element_id,
      NodeBiMap & second_order_node_bimap);

  /**
   * Fixes the node ordering for hex27 second-order mesh elements. This is called
   * internally in handleQuadraticFESpace. This method should be called only after
   * the Nodes have been created and their coordinates have been set.
   */
  void applyCorrectionsToHex27Elements(
      mfem::FiniteElementSpace & finite_element_space,
      std::map<int, std::array<double, 3>> & coordinates_for_libmesh_node_id,
      std::map<int, std::vector<int>> & libmesh_node_ids_for_element_id,
      std::map<int, std::array<int, 6>> & libmesh_center_of_face_node_ids_for_hex27_element_id,
      NodeBiMap & second_order_node_bimap);

  /**
   * Corrects the center node for each hex27 face. Called in applyCorrectionsToHex27Elements.
   */
  void applyCenterOfFacesCorrectionForHex27Element(
      const int ielement,
      mfem::FiniteElementSpace & finite_element_space,
      std::map<int, std::array<double, 3>> & coordinates_for_libmesh_node_id,
      std::map<int, std::vector<int>> & libmesh_node_ids_for_element_id,
      std::map<int, std::array<int, 6>> & libmesh_center_of_face_node_ids_for_hex27_element_id,
      NodeBiMap & second_order_node_bimap);

  /**
   * Corrects the interior node of a hex27 element. called in applyCorrectionsToHex27Elements.
   */
  void applyInteriorNodeCorrectionForHex27Element(
      const int ielement,
      mfem::FiniteElementSpace & finite_element_space,
      std::map<int, std::array<double, 3>> & coordinates_for_libmesh_node_id,
      std::map<int, std::vector<int>> & libmesh_node_ids_for_element_id,
      NodeBiMap & second_order_node_bimap);

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
      std::map<int, std::array<double, 3>> & coordinates_for_libmesh_node_id,
      NodeBiMap & second_order_node_bimap);

  /**
   * Writes face, node and edge information for all 2nd order elements to a file
   * and closes the file.
   */
  void writeFaceInfoFor2ndOrderElements(const char * fpath,
                                        mfem::FiniteElementSpace * fe_space) const;

  void writeNodeInfoFor2ndOrderElements(const char * fpath,
                                        mfem::FiniteElementSpace * fe_space) const;

  void writeEdgeInfoFor2ndOrderElements(const char * fpath,
                                        mfem::FiniteElementSpace * fe_space) const;

  /**
   * Write face, node and edge information for the specified 2nd order element
   * to a file. NB: the file will not be closed after the call. The caller will
   * have to call fclose() manually.
   */
  void writeFaceInfoFor2ndOrderElement(FILE * fp,
                                       mfem::FiniteElementSpace * fe_space,
                                       const int ielement) const;

  void writeNodeInfoFor2ndOrderElement(FILE * fp,
                                       mfem::FiniteElementSpace * fe_space,
                                       const int ielement) const;

  void writeEdgeInfoFor2ndOrderElement(FILE * fp,
                                       mfem::FiniteElementSpace * fe_space,
                                       const int ielement) const;

  /**
   * Map from the MFEM element id to the libmesh element id internally.
   */
  std::map<int, int> _libmesh_element_id_for_mfem_element_id;

  /**
   * Maps from the libmesh corner node id to the mfem vertex id internally.
   */
  std::map<int, int> _mfem_vertex_index_for_libmesh_corner_node_id;
};
