#include "auxiliary_variables.hpp"

namespace hephaestus
{

AuxiliaryVariable::AuxiliaryVariable()
{
}

AuxiliaryVariable::AuxiliaryVariable(const std::string& var_name,
                                     const std::string& var_family,
                                     int var_order,
                                     mfem::FiniteElementSpace& fespace)
    :name(var_name),
    family(var_family),
    order(var_order),
    gf(&fespace)
{
}


