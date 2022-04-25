#pragma once

#include "mfem.hpp"


class InheritedMFEMMesh : public mfem::Mesh 
{
public:
    InheritedMFEMMesh(const mfem::Vector &points, const mfem::Array<int> &cell_data,
        const mfem::Array<int> &cell_offsets,
        const mfem::Array<int> &cell_types,
        const mfem::Array<int> &cell_attributes,
        int &curved, int &read_gf, bool &finalize_topo);


    InheritedMFEMMesh(std::string afilename, int generate_edges = 0, int refine = 1, bool fix_orientation = true);

    void get_mesh_nodes();

    void get_connectivity_data();

    void get_cell_type();

    int get_num_nodes();
    
    std::vector<int> connectivityVec;
    std::vector<double> pointsVec;
    std::vector<int> cellTypeVec;
    std::vector<int> verticesVec;

};

    
