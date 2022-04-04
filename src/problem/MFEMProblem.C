#include "MFEMProblem.h"
#include "SystemBase.h"
#include "Transient.h"
#include "MFEMFunctionDirichletBC.h"
#include "hephaestus.hpp"


registerMooseObject("ApolloApp", MFEMProblem);


InputParameters MFEMProblem::validParams()
{
  InputParameters params = ExternalProblem::validParams();
  params.addParam<std::string>("input_mesh", "Input mesh for MFEM.");
  params.addParam<std::string>("formulation", "Name of EM formulation to use in MFEM.");
  params.addParam<int>("order", "Order of the FE variables for MFEM.");
  params.addParam<double>("dt", "Time step");
  params.addParam<double>("end_time", "Time at which to end transient simulation.");

  return params;
}


MFEMProblem::MFEMProblem(const InputParameters & params)
  : ExternalProblem(params),
    _input_mesh(_mesh.parameters().get<MeshFileName>("file")),
    _formulation(getParam<std::string>("formulation")),
    _order(getParam<int>("order")),
    _bc_maps(),
    _mat_map(),
    _executioner(std::string("Transient"), getParam<double>("dt"), getParam<double>("end_time")),
    _outputs()
{
}

void MFEMProblem::syncSolutions(Direction direction)
{
  //If data is being sent back to master app
  if(direction == Direction::FROM_EXTERNAL_APP)
  {
    for(auto name: getVariableNames())
    {
      setMOOSEVarData(_eq, _var_map[name]);
    }
  }

  //If data is being sent from the master app
  if(direction == Direction::TO_EXTERNAL_APP)
  {
    for(std::string name: getVariableNames())
    {
      setMFEMVarData(_eq, _var_map[name]);
    }
  }
}

void
MFEMProblem::addBoundaryCondition(const std::string & bc_name,
                                  const std::string & name,
                                  InputParameters & parameters)
{
  FEProblemBase::addUserObject(bc_name, name, parameters);
  MFEMBoundaryCondition * mfem_bc(&getUserObject<MFEMBoundaryCondition>(name));
  _bc_maps[name] = mfem_bc->getBC();
}


void MFEMProblem::addMaterial(const std::string & kernel_name,
                  const std::string & name,
                  InputParameters & parameters)
{
  FEProblemBase::addUserObject(kernel_name, name, parameters);
  const MFEMMaterial & mfem_material(getUserObject<MFEMMaterial>(name));

  for (unsigned int bid = 0; bid < mfem_material.blocks.size(); ++bid)
  {
    int block = std::stoi(mfem_material.blocks[bid]);
    hephaestus::Subdomain mfem_subdomain(name, block);
    mfem_subdomain.property_map = mfem_material.scalar_property_map;
    _mat_map.subdomains.push_back(mfem_subdomain);
  }

}


void MFEMProblem::addAuxVariable(const std::string& var_type, 
                                 const std::string& var_name,
                                 InputParameters& parameters)
{
  //Standard Moose implementation
  auto var_order = Utility::string_to_enum<Order>(parameters.get<MooseEnum>("order"));
  std::string var_family = parameters.get<MooseEnum>("family");
  auto fe_type = FEType(var_order, Utility::string_to_enum<FEFamily>(var_family));

  if (duplicateVariableCheck(var_name, fe_type, /* is_aux = */ true))
  {
    return;
  }
    
  parameters.set<FEProblemBase *>("_fe_problem_base") = this;
  parameters.set<Moose::VarKindType>("_var_kind") = Moose::VarKindType::VAR_AUXILIARY;  
  _aux->addVariable(var_type, var_name, parameters);

  if (_displaced_problem)
  {
    _displaced_problem->addAuxVariable(var_type, var_name, parameters);
  }
  //End of standard implementation

  //New code to create MFEM grid functions
  mfem::Mesh& mesh = getMFEMMesh().other_mesh;
  mfem::FiniteElementCollection* fec = fecGet(var_family);
  // mfem::H1_FECollection* fec = new mfem::H1_FECollection(_order, mesh.Dimension());
  // mfem::H1_FECollection fec(_order, mesh.Dimension());
  mfem::FiniteElementSpace fespace(&mesh, fec);
  hephaestus::AuxiliaryVariable* var = new hephaestus::AuxiliaryVariable(var_name, var_family, (int)var_order, fespace);
  _var_map.insert(std::pair<std::string, hephaestus::AuxiliaryVariable*>(var_name, var));
  // std::cout << "Add Aux Var " << var_name << " successful, family = " << var_family << std::endl;
  
}


void MFEMProblem::setMFEMVarData(EquationSystems& esRef, hephaestus::AuxiliaryVariable* var)
{
  auto & mooseVarRef = getVariable(0, var->name);

  NumericVector<Number>& tempSolutionVector = mooseVarRef.sys().solution();
  for(int i = 0; i < mesh().getMesh().n_nodes() /*number of nodes*/; i++)
  {
    Node * nodePtr = mesh().getMesh().node_ptr(i);
    dof_id_type dof = nodePtr->dof_number(mooseVarRef.sys().number(), mooseVarRef.number(), 0);
    var->gf[i] = tempSolutionVector(dof);
  }
  mooseVarRef.sys().solution().close();
  mooseVarRef.sys().update();
}


void MFEMProblem::setMOOSEVarData(EquationSystems& esRef, hephaestus::AuxiliaryVariable* var)
{
  auto & mooseVarRef = getVariable(
      0, var->name, Moose::VarKindType::VAR_ANY, Moose::VarFieldType::VAR_FIELD_STANDARD);

  for(int i = 0; i < mesh().getMesh().n_nodes(); i++)
  {
    Node * nodePtr = mesh().getMesh().node_ptr(i);
    dof_id_type dof = nodePtr->dof_number(mooseVarRef.sys().number(), mooseVarRef.number(), 0);
    mooseVarRef.sys().solution().set(dof, var->gf[i]); /*Needs to be changed for tetra*/
  }
  mooseVarRef.sys().solution().close();
  mooseVarRef.sys().update();
}


mfem::FiniteElementCollection* MFEMProblem::fecGet(std::string var_fam)
{
  mfem::Mesh& mesh = getMFEMMesh().other_mesh;
  mfem::FiniteElementCollection* fecPtr;
  std::cout << "Variable family = " << var_fam << std::endl;

  if(var_fam == "LAGRANGE")
  {
    mfem::H1_FECollection* fec = new mfem::H1_FECollection(_order, mesh.Dimension());
    fecPtr = dynamic_cast<mfem::FiniteElementCollection*>(fec);
  }

  if(var_fam == "NEDELEC_ONE")
  {
    mfem::ND1_3DFECollection* fec = new mfem::ND1_3DFECollection();
    fecPtr = dynamic_cast<mfem::FiniteElementCollection*>(fec);
  }
  //More types need adding, I need to understand what types are analogous 

  return fecPtr;
}








