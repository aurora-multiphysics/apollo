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
class CoupledMFEMMesh : public ExclusiveMFEMMesh {
 public:
  static InputParameters validParams();

  CoupledMFEMMesh(const InputParameters& parameters);

  virtual ~CoupledMFEMMesh();

  virtual std::unique_ptr<MooseMesh> safeClone() const override;

  virtual void buildMesh() override;
  
  void getElementInfo();

  void getBdrLists(int** elem_ss, int** side_ss);

  int getNumSidesets();
  
  void createMFEMMesh();

  void create_ss_node_id(int** elem_ss, int** side_ss, int** ss_node_id);

 protected:

  int curved = 0;
  int read_gf = 0;
  bool topo = false;

  unsigned int num_node_per_el;
  int libmesh_element_type;
  int libmesh_face_type;
  int num_element_linear_nodes;
  int num_face_nodes;
  int num_face_linear_nodes;
  int num_side_sets;
  int bdrElems;
  std::vector<int> num_sides_in_ss;

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
