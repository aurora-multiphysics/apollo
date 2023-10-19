#pragma once
#include <map>

/**
 * BiMap
 *
 * Simple mapping between any two types.
 */
template <typename TKey, typename TValue>
class BiMap
{
public:
  BiMap() = default;
  ~BiMap() = default;

  inline TKey getKey(const TValue value) { return _key_for_value[value]; }
  inline TValue getValue(const TKey key) { return _value_for_key[key]; }

  inline void insert(const TKey key, const TValue value)
  {
    _key_for_value[value] = key;
    _value_for_key[key] = value;
  }

  inline void clear()
  {
    _key_for_value.clear();
    _value_for_key.clear();
  }

protected:
  std::map<TKey, TValue> _key_for_value;
  std::map<TValue, TKey> _value_for_key;
};

/**
 * NodeBiMap
 *
 * Use this class for mapping between MFEM and libMesh node ids.
 */
class NodeBiMap : protected BiMap<int, int>
{
public:
  inline int getMFEMNodeID(const int libmesh_node_id) { return getKey(libmesh_node_id); }
  inline int getLibmeshNodeID(const int mfem_node_id) { return getValue(mfem_node_id); }

  inline void insertNodeIDs(const int mfem_node_id, const int libmesh_node_id)
  {
    insert(mfem_node_id, libmesh_node_id); // (key, value).
  }

  inline void clearNodeIDs() { clear(); }
};
