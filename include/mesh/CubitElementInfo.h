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
    FACE_QUAD9
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
    ELEMENT_HEX27
  }; // TODO: - extend to Wedge6, Wedge15 element types.

  inline CubitElementType getElementType() const { return _element_type; }

  /**
   * Returns constant reference to face info.
   */
  inline const CubitFaceInfo & getFaceInfo() const { return _info_for_face.front(); }

  /**
   * Returns info for a particular face.
   */
  const CubitFaceInfo & getFaceInfo(int iface) const;

  inline uint8_t getNumNodes() const { return _num_nodes; }
  inline uint8_t getNumCornerNodes() const { return _num_corner_nodes; }

  inline uint8_t getOrder() const { return _order; }
  inline uint8_t getDimension() const { return _dimension; }

protected:
  void buildCubit2DElementInfo(int num_nodes_per_element);
  void buildCubit3DElementInfo(int num_nodes_per_element);

  /**
   * Stores element type.
   */
  CubitElementType _element_type;

  /**
   * Stores info about the face types (assume all faces are identical!)
   */
  std::vector<CubitFaceInfo> _info_for_face; /*
    * NB: "corner nodes" refer to MOOSE nodes at the corners of an element. In
   /**
    * NB: "corner nodes" refer to MOOSE nodes at the corners of an element. In
   * MFEM this is referred to as "vertices".
   */
  uint8_t _num_nodes;
  uint8_t _num_corner_nodes;

  /**
   * NB: first-order elements have only nodes on the "corners". Second-order have
   * additional nodes between "corner" nodes.
   */
  uint8_t _order;
  uint8_t _dimension;
};
