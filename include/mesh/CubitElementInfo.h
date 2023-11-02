#pragma once
#include <stdint.h>
#include "MooseError.h"

class CubitElementInfo
{
public:
  CubitElementInfo() = default;
  ~CubitElementInfo() = default;

  CubitElementInfo(int num_nodes_per_element, int dimension = 3);

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

  inline CubitElementType getElementType() const { return _element_type; }
  inline CubitFaceType getFaceType() const { return _face_type; }

  inline uint8_t getNumNodes() const { return _num_nodes; }
  inline uint8_t getNumCornerNodes() const { return _num_corner_nodes; }

  inline uint8_t getNumFaceNodes() const { return _num_face_nodes; }
  inline uint8_t getNumFaceCornerNodes() const { return _num_face_corner_nodes; }

  inline uint8_t getOrder() const { return _order; }
  inline uint8_t getDimension() const { return _dimension; }

protected:
  void buildCubit2DElementInfo(int num_nodes_per_element);
  void buildCubit3DElementInfo(int num_nodes_per_element);
  void buildCubitFaceInfo();

  CubitElementType _element_type;
  CubitFaceType _face_type;

  /**
   * NB: "corner nodes" refer to MOOSE nodes at the corners of an element. In
   * MFEM this is referred to as "vertices".
   */
  uint8_t _num_nodes;
  uint8_t _num_corner_nodes;

  uint8_t _num_face_nodes;
  uint8_t _num_face_corner_nodes;

  /**
   * NB: first-order elements have only nodes on the "corners". Second-order have
   * additional nodes between "corner" nodes.
   */
  uint8_t _order;
  uint8_t _dimension;
};
