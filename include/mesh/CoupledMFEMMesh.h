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

  void getBdrLists(std::vector<std::vector<int>> & elem_ss,
                   std::vector<std::vector<int>> & side_ss);

  int getNumSidesets();

  /**
   * Returns a unique pointer to a C partitioning array.
   */
  std::unique_ptr<int[]> getMeshPartitioning() const;

  void createSidesetNodeIDs(const std::vector<std::vector<int>> & elem_ss,
                            const std::vector<std::vector<int>> & side_ss,
                            std::vector<std::vector<int>> & ss_node_id);

  std::map<int, int> _libmesh_to_mfem_node_map;

protected:
  void buildLibmesh3DElementInfo();
  void buildLibmesh2DElementInfo();
  void buildLibmeshElementInfo();

  void buildLibmeshFaceInfo();

  void buildLibmeshElementAndFaceInfo();

  // Override methods in Exclusive MFEMMesh.
  void buildMFEMMesh() override;
  void buildMFEMParMesh() override;

  std::vector<int> _num_sides_in_ss;

  unsigned int _num_nodes_per_element;
  int _libmesh_element_type;
  int _libmesh_face_type;
  int _num_linear_nodes_per_element;
  int _num_face_nodes;
  int _num_face_linear_nodes;
  int _num_sidesets;

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
};
