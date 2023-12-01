#include "CubitElementInfo.h"

/**
 * CubitFaceInfo
 */
CubitFaceInfo::CubitFaceInfo(CubitFaceType face_type) : _face_type(face_type)
{
  buildCubitFaceInfo();
}

void
CubitFaceInfo::buildCubitFaceInfo()
{
  switch (_face_type)
  {
    case (FACE_EDGE2):
    {
      _num_face_nodes = 2;
      _num_face_corner_nodes = 2;
      break;
    }
    case (FACE_EDGE3):
    {
      _num_face_nodes = 3;
      _num_face_corner_nodes = 2;
      break;
    }
    case (FACE_TRI3):
    {
      _num_face_nodes = 3;
      _num_face_corner_nodes = 3;
      break;
    }
    case (FACE_TRI6):
    {
      _num_face_nodes = 6;
      _num_face_corner_nodes = 3;
      break;
    }
    case (FACE_QUAD4):
    {
      _num_face_nodes = 4;
      _num_face_corner_nodes = 4;
      break;
    }
    case (FACE_QUAD9):
    {
      _num_face_nodes = 9;
      _num_face_corner_nodes = 4;
      break;
    }
    default:
    {
      mooseError("Unsupported face type '", _face_type, "'.");
      break;
    }
  }
}

/**
 * CubitElementInfo
 */
CubitElementInfo::CubitElementInfo(int num_nodes_per_element, int dimension)
{
  switch (dimension)
  {
    case 2:
    {
      buildCubit2DElementInfo(num_nodes_per_element);
      break;
    }
    case 3:
    {
      buildCubit3DElementInfo(num_nodes_per_element);
      break;
    }
    default:
    {
      mooseError("Unsupported element dimension ", dimension, ".\n");
      break;
    }
  }
}

void
CubitElementInfo::buildCubit2DElementInfo(int num_nodes_per_element)
{
  _dimension = 2;
  _num_nodes = num_nodes_per_element;
  _info_for_face.clear();

  switch (num_nodes_per_element)
  {
    case 3:
    {
      _element_type = ELEMENT_TRI3;
      _info_for_face.push_back(CubitFaceInfo(CubitFaceInfo::FACE_EDGE2));
      _num_faces = 3;
      _num_corner_nodes = 3;
      _order = 1;
      break;
    }
    case 6:
    {
      _element_type = ELEMENT_TRI6;
      _info_for_face.push_back(CubitFaceInfo(CubitFaceInfo::FACE_EDGE3));
      _num_faces = 3;
      _num_corner_nodes = 3;
      _order = 2;
      break;
    }
    case 4:
    {
      _element_type = ELEMENT_QUAD4;
      _info_for_face.push_back(CubitFaceInfo(CubitFaceInfo::FACE_EDGE2));
      _num_faces = 4;
      _num_corner_nodes = 4;
      _order = 1;
      break;
    }
    case 9:
    {
      _element_type = ELEMENT_QUAD9;
      _info_for_face.push_back(CubitFaceInfo(CubitFaceInfo::FACE_EDGE3));
      _num_faces = 4;
      _num_corner_nodes = 4;
      _order = 2;
      break;
    }
    default:
    {
      mooseError("Unsupported 2D element with ", num_nodes_per_element, " nodes per element.\n");
      break;
    }
  }
}

void
CubitElementInfo::buildCubit3DElementInfo(int num_nodes_per_element)
{
  _dimension = 3;
  _num_nodes = num_nodes_per_element;
  _info_for_face.clear();

  switch (num_nodes_per_element)
  {
    case 4:
    {
      _element_type = ELEMENT_TET4;
      _info_for_face.push_back(CubitFaceInfo(CubitFaceInfo::FACE_TRI3));
      _num_faces = 4;
      _num_corner_nodes = 4;
      _order = 1;
      break;
    }
    case 10:
    {
      _element_type = ELEMENT_TET10;
      _info_for_face.push_back(CubitFaceInfo(CubitFaceInfo::FACE_TRI6));
      _num_faces = 4;
      _num_corner_nodes = 4;
      _order = 2;
      break;
    }
    case 8:
    {
      _element_type = ELEMENT_HEX8;
      _info_for_face.push_back(CubitFaceInfo(CubitFaceInfo::FACE_QUAD4));
      _num_faces = 6;
      _num_corner_nodes = 8;
      _order = 1;
      break;
    }
    case 27:
    {
      _element_type = ELEMENT_HEX27;
      _info_for_face.push_back(CubitFaceInfo(CubitFaceInfo::FACE_QUAD9));
      _num_faces = 6;
      _num_corner_nodes = 8;
      _order = 2;
      break;
    }
    default:
    {
      mooseError("Unsupported 3D element with ", num_nodes_per_element, " nodes per element.\n");
      break;
    }
  }
}

const CubitFaceInfo &
CubitElementInfo::getFaceInfo(int iface) const
{
  /**
   * Check _info_for_face initialized.
   */
  if (_info_for_face.empty())
  {
    mooseError("_info_for_face is empty.");
  }

  /**
   * Check valid face index.
   */
  bool is_valid_face_index = (iface >= 0 && iface < _num_faces);
  if (!is_valid_face_index)
  {
    mooseError("Face index '", iface, "' is invalid.");
  }

  /**
   * Case 1: single face type --> only store a single face.
   * Case 2: multiple face types --> store each face. Return face at index.
   */
  bool is_single_face_type = (_info_for_face.size() == 1);

  /**
   * Check vector size matches _num_Faces for multiple face types.
   */
  if (!is_single_face_type && _info_for_face.size() != _num_faces)
  {
    mooseError("_info_for_face.size() != _num_faces.");
  }

  return is_single_face_type ? _info_for_face.front() : _info_for_face[iface];
};
