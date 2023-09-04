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
           int num_linear_nodes_per_element,
           int num_face_nodes,
           int num_face_linear_nodes,
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
   * Returns a constant reference to the protected libmesh_to_mfem_node_map.
   */
  const std::map<int, int> & getLibmeshToMFEMNodeMap() const;

protected:
  std::map<int, int> _libmesh_to_mfem_node_map;

  const int getOrderFromLibmeshElementType(int libmesh_element_type) const;

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
