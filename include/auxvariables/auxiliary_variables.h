#pragma once
#include <memory>
#include <iostream>
#include <fstream>
#include "mesh_extras.hpp"


namespace hephaestus
{

class AuxiliaryVariable
{
    public:
    AuxiliaryVariable();
    AuxiliaryVariable(const std::string& var_name, 
                      const std::string& var_family,
                      int var_order,
                      mfem::FiniteElementSpace& fespace);
    
    std::string name;
    std::string family;
    int order;
    mfem::GridFunction gf;
};


class AuxVarMap : public std::map<std::string, hephaestus::AuxiliaryVariable *>{

};
// {
//     public:
//     AuxVarMap();
//     void setAuxVar(std::string var_name, AuxiliaryVariable auxVar);
//     AuxiliaryVariable getAuxVar(std::string var_name);
//     std::map<std::string, hephaestus::AuxiliaryVariable*> var_map;
// };

}