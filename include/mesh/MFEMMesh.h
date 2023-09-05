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
  MFEMMesh(int num_elements_in_mesh,
           std::map<int, std::array<double, 3>> & coordinates_for_unique_corner_node_id,
           std::map<int, int> & libmesh_to_mfem_corner_node_id_map,
           std::vector<int> & unique_corner_node_ids,
           int libmesh_element_type,
           int libmesh_face_type,
           std::map<int, std::vector<int>> & element_ids_for_block_id,
           std::map<int, std::vector<int>> & node_ids_for_element_id,
           int num_corner_nodes_per_element,
           int num_face_nodes,
           int num_face_corner_nodes,
           std::map<int, int> & num_elements_for_boundary_id,
           std::map<int, std::vector<int>> & node_ids_for_boundary_id,
           const std::vector<int> & unique_block_ids,
           const std::vector<int> & unique_side_boundary_ids,
           int num_dimensions);

  MFEMMesh(std::string mesh_fname,
           int generate_edges = 0,
           int refine = 1,
           bool fix_orientation = true);

  /**
   * Returns a constant reference to the protected _libmesh_to_mfem_node_map
   * member variable.
   */
  const std::map<int, int> & getLibmeshToMFEMNodeMap() const;

protected:
  /**
   * Sets the protected variable array using the provided vector of corner node
   * IDs from MOOSE. Note that the vertices (named "nodes" in MOOSE) are ONLY
   * at the corners of elements. These are referred to as "corner nodes" in MOOSE.
   */
  void
  buildMFEMVertices(std::vector<int> & unique_corner_node_ids,
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
                         std::map<int, std::vector<int>> & node_ids_for_element_id,
                         std::map<int, int> & index_for_unique_corner_node_id);

  /**
   * Construct the boundary array of elements.
   */
  void buildMFEMBoundaryElements(const int libmesh_face_type,
                                 const int num_face_nodes,
                                 const int num_face_corner_nodes,
                                 const std::vector<int> & unique_side_boundary_ids,
                                 std::map<int, int> & num_elements_for_boundary_id,
                                 std::map<int, std::vector<int>> & node_ids_for_boundary_id,
                                 std::map<int, int> & index_for_unique_corner_node_id);

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
      std::map<int, std::array<double, 3>> & coordinates_for_unique_corner_node_id);

  /**
   * Determines the order from the libmesh element type provided.
   */
  const int getOrderFromLibmeshElementType(int libmesh_element_type) const;

  std::map<int, int> _libmesh_to_mfem_node_map;

  enum CubitFaceType // TODO: - this should not be duplicated.
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
};
