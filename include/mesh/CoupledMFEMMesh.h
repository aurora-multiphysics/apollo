//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once
#include "ExclusiveMFEMMesh.h"
#include "CubitElementInfo.h"
#include "NodeBiMap.h"

/**
 * CoupledMFEMMesh
 */
class CoupledMFEMMesh : public ExclusiveMFEMMesh
{
public:
  static InputParameters validParams();

  CoupledMFEMMesh(const InputParameters & parameters);

  virtual ~CoupledMFEMMesh();

  std::unique_ptr<MooseMesh> safeClone() const override;

  /**
   * Builds only the MOOSE mesh from the file.
   */
  void buildMesh() override;

  /**
   * Override method in ExclusiveMFEMMesh.
   */
  inline int getMFEMNodeID(const int libmesh_node_id) override
  {
    return _second_order_node_bimap.getMFEMNodeID(libmesh_node_id);
  }

  inline int getLibmeshNodeID(const int mfem_node_id) override
  {
    return _second_order_node_bimap.getLibmeshNodeID(mfem_node_id);
  }

protected:
  /**
   * An internal method used to create maps from each boundary ID to vectors of side IDs
   * and element IDs.
   */
  void buildBoundaryInfo(std::map<int, std::vector<int>> & element_ids_for_boundary_id,
                         std::map<int, std::vector<int>> & side_ids_for_boundary_id,
                         std::map<int, int> & num_elements_for_boundary_id);

  /**
   * Create a mapping from each boundary ID to a vector containing the global node
   * IDs of nodes that lie on the faces of elements that fall on the boundary.
   */
  void buildBoundaryNodeIDs(const std::vector<int> & unique_side_boundary_ids,
                            std::map<int, std::vector<int>> & element_ids_for_boundary_id,
                            std::map<int, std::vector<int>> & side_ids_for_boundary_id,
                            std::map<int, std::vector<int>> & node_ids_for_boundary_id);

  /**
   * Builds two maps:
   * 1. Mapping from each block ID --> vector containing all element IDs for block.
   * 2. Mapping from each element --> vector containing all global node IDs for element.
   */
  void buildElementAndNodeIDs(const std::vector<int> & unique_block_ids,
                              std::map<int, std::vector<int>> & element_ids_for_block_id,
                              std::map<int, std::vector<int>> & node_ids_for_element_id);

  /**
   * Iterates through each block to find the elements in the block. For each
   * element in a block, it runs through the nodes in the block and adds only
   * the corner nodes to a vector. This is then sorted and only unique global
   * node IDs are retained.
   */
  void buildUniqueCornerNodeIDs(std::vector<int> & unique_corner_node_ids,
                                const std::vector<int> & unique_block_ids,
                                std::map<int, std::vector<int>> & element_ids_for_block_id,
                                std::map<int, std::vector<int>> & node_ids_for_element_id);
  /**
   * Sets an array with the global node IDs of the six nodes that lie on the
   * center of each face. This is required for fixing issues with the hex27
   * element. Returns a boolean.
   */
  bool buildHex27CenterOfFaceNodeIDsForElement(const int element_id,
                                               std::array<int, 6> & center_of_face_node_ids);

  /**
   * Sets a map from the element id to an array containing the global node IDs
   * of nodes that lie on the center of each of the element's faces. Returns a
   * boolean.
   */
  bool buildHex27CenterOfFaceNodeIDs(
      std::map<int, std::vector<int>> & element_ids_for_block_id,
      std::map<int, std::array<int, 6>> & center_of_face_node_ids_for_hex27_element_id);

  /**
   * Sets protected member variables.
   */
  void buildLibmeshElementAndFaceInfo();

  /**
   * Blocks/subdomains are separate subsets of the mesh that could have different
   * material properties etc. This method returns a vector containing the unique
   * IDs of each block in the mesh. This will be passed to the MFEMMesh constructor
   * which sets the attribute of each element to the ID of the block that it is a
   * part of.
   */
  std::vector<int> getLibmeshBlockIDs() const;

  /**
   * Returns a vector containing the IDs of all boundaries.
   */
  std::vector<int> getSideBoundaryIDs() const;

  /**
   * Returns a pointer to the first element on the processor.
   */
  const Elem * getFirstElementOnProcessor() const;

  /**
   * Returns the libMesh partitioning. The "raw" pointer is wrapped up in a unique
   * pointer.
   */
  std::unique_ptr<int[]> getMeshPartitioning();

  /**
   * Returns true if mesh is split between two or more processors.
   */
  bool isDistributedMesh() const;

  /**
   * Override methods in Exclusive MFEMMesh.
   */
  void buildMFEMMesh() override;
  void buildMFEMParMesh() override;

  /**
   * The element and face type used in the MOOSE mesh. We currently only support
   * a single element type. Support for additional element types in a mesh will
   * be added in the future.
   */
  CubitElementInfo _element_info;

  /**
   * MFEM <--> libMesh bimap required for higher-order mesh element transfers.
   */
  NodeBiMap _second_order_node_bimap;
};
