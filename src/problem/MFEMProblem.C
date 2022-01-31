#include "MFEMProblem.h"
#include "SystemBase.h"

#include "hephaestus_joule.hpp"


registerMooseObject("ApolloApp", MFEMProblem);


InputParameters MFEMProblem::validParams()
{
  InputParameters params = ExternalProblem::validParams();
  params.addParam<std::string>("input_mesh", "Input mesh for MFEM. "
  "Should be consistent with problem type.");
  params.addParam<std::string>("problem_type", "Problem type for Joule solver. "
  "Should be 'rod' or 'coil'.");
  return params;
}

MFEMProblem::MFEMProblem(const InputParameters & params)
  : ExternalProblem(params),
  _problem_type(getParam<std::string>("problem_type")),
  _input_mesh(getParam<std::string>("input_mesh"))
{

}

void MFEMProblem::externalSolve(){
    std::cout << "Launching MFEM solve\n\n" << std::endl;
    std::cout << "Proof: " << _input_mesh << std::endl;

    MeshBase &meshy = mesh().getMesh();
    std::cout << meshy.spatial_dimension() << std::endl;

    std::vector<std::string> arguments = {"joule", "-m", _input_mesh, "-o", "2", "-p", _problem_type};

    std::vector<char*> argv;
    for (const auto& arg : arguments)
        argv.push_back((char*)arg.data());
    argv.push_back(nullptr);

    joule_solve(argv.size() - 1, argv.data());
}


/*ExternalPETScProblem::syncSolutions(Direction direction)
{
  if (direction == Direction::TO_EXTERNAL_APP)
  {
    _console << "syncSolutions to MFEM App" << std::endl;

    // Take the solution from PETSc, and sync it to one MOOSE variable
    // We currently support one variable only but it is straightforward
    // to have multiple moose variables
    MeshBase & to_mesh = mesh().getMesh();
    auto & sync_to_var = getVariable(
        0, _sync_to_var_name, Moose::VarKindType::VAR_ANY, Moose::VarFieldType::VAR_FIELD_STANDARD);

    for (j = ys; j < ys + ym; j++)
      for (i = xs; i < xs + xm; i++)
      {
        Node * to_node = to_mesh.node_ptr(i + j * Mx);



        // For the current example, we need to update only one variable.
        // This line of code is used to make sure users won't make a mistake in the demo input file.
        // If multiple variables need to be transfered for some use cases, users should
        // loop over variables and copy necessary data.



        if (to_node->n_comp(sync_to_var.sys().number(), sync_to_var.number()) > 1)
          mooseError("Does not support multiple components");

        dof_id_type dof = to_node->dof_number(sync_to_var.sys().number(), sync_to_var.number(), 0);
        // Copy the solution to the right location
        sync_to_var.sys().solution().set(dof, _petsc_sol_array[j][i]);
      }

    sync_to_var.sys().solution().close();

    DMDAVecRestoreArray(da, _petsc_sol, &_petsc_sol_array);

    // Make the solution and the current local solution consistent
    sync_to_var.sys().update();
  }
  else if (direction == Direction::TO_EXTERNAL_APP)
  {
    _console << "syncSolutions to external petsc App " << std::endl;
    // We could the similar thing to sync the solution back to PETSc.
  }
} */
