#pragma once
<<<<<<< HEAD

#include "mfem.hpp"

class MFEMMesh : public mfem::Mesh {
 public:
  MFEMMesh(const mfem::Vector &points, const mfem::Array<int> &cell_data,
           const mfem::Array<int> &cell_offsets,
           const mfem::Array<int> &cell_types,
           const mfem::Array<int> &cell_attributes, int &curved, int &read_gf,
           bool &finalize_topo);

  MFEMMesh(std::string afilename, int generate_edges = 0, int refine = 1,
           bool fix_orientation = true);

  void get_mesh_nodes();

  void get_connectivity_data();

  void get_cell_type();
=======
#include "MooseMesh.h"
#include "mfem.hpp"
#include "InheritedMFEMMesh.h"
#include "libmesh/vtk_io.h"
#include "libmesh/equation_systems.h"
#include "libmesh/libmesh_config.h"
#include "libmesh/mesh_base.h"
#include "libmesh/mesh_input.h"
#include "libmesh/numeric_vector.h"
#include "libmesh/system.h"
#include "libmesh/node.h"
#include "libmesh/elem.h"
#include "libmesh/enum_io_package.h"
#include "libmesh/ignore_warnings.h"
#include "libmesh/numeric_vector.h"

/**
 * MFEMMesh
 */
class MFEMMesh : public MooseMesh
{
public:
  static InputParameters validParams();

  MFEMMesh(const InputParameters & parameters);
  

  virtual void buildMesh() override;

  //Builds placeholder mesh when no MOOSE mesh is required
  void buildDummyMesh();
  //Builds the corresponding MOOSE mesh
  void buildRealMesh();

  //Function that maps vtk element types to libmesh element types
  int map_elems_vtk_to_libmesh(int VTKElemType);

  //Mesh dimension
  int dim;

  //The object holding our MFEM mesh. Needs renaming as to avoid confusion
  InheritedMFEMMesh other_mesh;
  
  //Boolean value determining whether the user wants the corresponding MOOSE mesh built
  bool CreateMOOSEMesh;

  virtual std::unique_ptr<MooseMesh> safeClone() const override;

  
};
>>>>>>> 44e4f52 (Updated meshing functionality, added MOOSE mesh creation within MFEMMesh, added InheritedMFEMMesh class to allow for mfem meshes to be instantiated from new constructors)

  int get_num_nodes();

  std::vector<int> connectivityVec;
  std::vector<double> pointsVec;
  std::vector<int> cellTypeVec;
  std::vector<int> verticesVec;
};
