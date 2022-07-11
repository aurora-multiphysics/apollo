#pragma once

#include "libmesh/elem.h"
#include "libmesh/enum_io_package.h"
#include "libmesh/exodusII_io.h"
#include "libmesh/nemesis_io.h"
#include "libmesh/node.h"
#include "libmesh/parallel_mesh.h"
#include "mfem.hpp"

class MFEMMesh : public mfem::Mesh {
 public:
  MFEMMesh(int num_elem, std::vector<double> &coordx,
                    std::vector<double> &coordy, std::vector<double> &coordz,
                    std::map<int, int> &cubitToMFEMVertMap,
                    std::vector<int> uniqueVertexID, int libmesh_element_type,
                    int libmesh_face_type, int **elem_blk, int num_el_blk,
                    int num_node_per_el, size_t *num_el_in_blk,
                    int num_element_linear_nodes, int num_face_nodes,
                    int num_face_linear_nodes, int num_side_sets,
                    std::vector<int> num_side_in_ss, int **ss_node_id,
                    int *eb_prop, int *ss_prop, int dim_num, int* start_of_block);

  MFEMMesh(std::string afilename, int generate_edges = 0,
                    int refine = 1, bool fix_orientation = true);

  void get_mesh_nodes();

  void get_connectivity_data();

  void get_cell_type();

  int get_num_nodes();

  void create_boundary(int **ss_node_id, int *num_sides_in_sidesets,
                       int num_side_sets,
                       std::vector<libMesh::boundary_id_type> ssprop,
                       int num_face_linear_nodes, int num_face_nodes,
                       int libmesh_face_type);

  std::vector<int> connectivityVec;
  std::vector<double> pointsVec;
  std::vector<int> cellTypeVec;
  std::vector<int> verticesVec;

  enum CubitFaceType {
    FACE_EDGE2,
    FACE_EDGE3,
    FACE_TRI3,
    FACE_TRI6,
    FACE_QUAD4,
    FACE_QUAD9
  };

  enum CubitElementType {
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
