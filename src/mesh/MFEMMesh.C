#pragma once
#include "InheritedMFEMMesh.h"

//Constructor to create an MFEM mesh from VTK data structures. These data structures are obtained by the 
//methods found in MFEMproblem
InheritedMFEMMesh::InheritedMFEMMesh(const mfem::Vector &points, const mfem::Array<int> &cell_data,
    const mfem::Array<int> &cell_offsets,
    const mfem::Array<int> &cell_types,
    const mfem::Array<int> &cell_attributes,
    int &curved, int &read_gf, bool &finalize_topo)
{
    SetEmpty();
    CreateVTKMesh(points, cell_data, cell_offsets, cell_types, cell_attributes, curved, read_gf, finalize_topo);
}


//Constructor to create an MFEM mesh from a file, currently just used for testing purposes
InheritedMFEMMesh::InheritedMFEMMesh(std::string afilename, int generate_edges, int refine, bool fix_orientation)
{
    std::cout << afilename << std::endl;
    const char* filename = afilename.c_str();
    SetEmpty();

    mfem::named_ifgzstream imesh(filename);
    if (!imesh)
    {
        // Abort with an error message.
        MFEM_ABORT("Mesh file not found: " << filename << '\n');
    }
    else
    {
        Load(imesh, generate_edges, refine, fix_orientation);
    }
}



void InheritedMFEMMesh::get_mesh_nodes()
{
    for (int i = 0; i < NumOfVertices; i++)
    {
        this->pointsVec.push_back(vertices[i](0));
        int j;
        for (j = 1; j < spaceDim; j++)
        {
        this->pointsVec.push_back(vertices[i](j));
        }
        for ( ; j < 3; j++)
        {
        this->pointsVec.push_back(0.0);
        }
    }
}


void InheritedMFEMMesh::get_connectivity_data()
{

    mfem::RefinedGeometry *RefG;
    int ref = 1;
    int np = 0;

    for (int i = 0; i < NumOfElements; i++)
    {
        const int *v = elements[i]->GetVertices();
        const int nv = elements[i]->GetNVertices();
        // std::cout << nv;
        mfem::Geometry::Type geom = elements[i]->GetGeometryType();
        const int *perm = mfem::VTKGeometry::VertexPermutation[geom];
        this->verticesVec.push_back(nv);
        for (int j = 0; j < nv; j++)
        {
            // std::cout << ' ' << v[perm ? perm[j] : j];
            this->connectivityVec.push_back(v[perm ? perm[j] : j]);
        }
        
    }
}


void InheritedMFEMMesh::get_cell_type()
{
    mfem::RefinedGeometry *RefG;
    int ref = 1;
    for (int i = 0; i < GetNE(); i++)
    {
        mfem::Geometry::Type geom = GetElementBaseGeometry(i);
        int nv = mfem::Geometries.GetVertices(geom)->GetNPoints();
        RefG = mfem::GlobGeometryRefiner.Refine(geom, ref, 1);
        mfem::Array<int> &RG = RefG->RefGeoms;
        int vtk_cell_type = mfem::VTKGeometry::Map[geom];

        for (int j = 0; j < RG.Size(); j += nv)
        {
            this->cellTypeVec.push_back(vtk_cell_type);
        }
    }
}

