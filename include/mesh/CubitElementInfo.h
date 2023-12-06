#pragma once
#include <stdint.h>
#include "MooseError.h"

// TODO: Later add another class on top which will store the element type for each block.

/**
 * CubitFaceInfo
 *
 * Stores information about a particular element face.
 */
class CubitFaceInfo
{
public:
  CubitFaceInfo() = default; // TODO: - delete this to avoid failing to initialize.
  ~CubitFaceInfo() = default;

  enum CubitFaceType
  {
    FACE_EDGE2,
    FACE_EDGE3,
    FACE_TRI3,
    FACE_TRI6,
    FACE_QUAD4,
    FACE_QUAD8, // order = 2.
    FACE_QUAD9  // order = 2. Center node.
  };

  /**
   * Default initializer.
   */
  CubitFaceInfo(CubitFaceType face_type);

  inline uint8_t order() const { return _order; };
  inline uint8_t numFaceNodes() const { return _num_face_nodes; };
  inline uint8_t numFaceCornerNodes() const { return _num_face_corner_nodes; };
  inline CubitFaceType faceType() const { return _face_type; }

protected:
  void buildCubitFaceInfo();

private:
  /**
   * Type of face.
   */
  CubitFaceType _face_type;

  /**
   * Total number of nodes and number of corner nodes ("vertices").
   */
  uint8_t _num_face_nodes;
  uint8_t _num_face_corner_nodes;

  /**
   * Order of face.
   */
  uint8_t _order;
};

/**
 * CubitElementInfo
 *
 * Stores information about a particular element.
 */
class CubitElementInfo
{
public:
  CubitElementInfo() = default;
  ~CubitElementInfo() = default;

  CubitElementInfo(int num_nodes_per_element, int dimension = 3);

  enum CubitElementType
  {
    ELEMENT_TRI3,
    ELEMENT_TRI6,
    ELEMENT_QUAD4,
    ELEMENT_QUAD9,
    ELEMENT_TET4,
    ELEMENT_TET10,
    ELEMENT_HEX8,
    ELEMENT_HEX27,
    ELEMENT_WEDGE6,
    ELEMENT_PYRAMID5
  };

  CubitElementInfo(CubitElementType element_type);

  inline CubitElementType getElementType() const { return _element_type; }

  /**
   * Returns info for a particular face.
   */
  const CubitFaceInfo & getFaceInfo(int iface = 0) const;

  inline uint8_t getNumFaces() const { return _num_faces; }

  inline uint8_t getNumNodes() const { return _num_nodes; }
  inline uint8_t getNumCornerNodes() const { return _num_corner_nodes; }

  inline uint8_t getOrder() const { return _order; }
  inline uint8_t getDimension() const { return _dimension; }

protected:
  void buildCubit2DElementInfo(int num_nodes_per_element);
  void buildCubit3DElementInfo(int num_nodes_per_element);

  /**
   * Sets the _face_info vector.
   */
  std::vector<CubitFaceInfo> getWedge6FaceInfo() const;
  std::vector<CubitFaceInfo> getPyramid5FaceInfo() const;

private:
  /**
   * Stores the element type.
   */
  CubitElementType _element_type;

  /**
   * NB: first-order elements have only nodes on the "corners". Second-order have
   * additional nodes between "corner" nodes.
   */
  uint8_t _order;
  uint8_t _dimension;

  /**
   * NB: "corner nodes" refer to MOOSE nodes at the corners of an element. In
   * MFEM this is referred to as "vertices".
   */
  uint8_t _num_nodes;
  uint8_t _num_corner_nodes;

  /**
   * Stores info about the face types.
   */
  uint8_t _num_faces;
  std::vector<CubitFaceInfo> _face_info;
};
