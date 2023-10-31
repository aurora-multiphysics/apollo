#include "ApolloApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"
#include "MooseSyntax.h"

InputParameters
ApolloApp::validParams()
{
  InputParameters params = MooseApp::validParams();
  params.set<bool>("use_legacy_material_output") = false;
  return params;
}

ApolloApp::ApolloApp(InputParameters parameters) : MooseApp(parameters)
{
  ApolloApp::registerAll(_factory, _action_factory, _syntax);
}

ApolloApp::~ApolloApp() {}

static void
associateSyntaxInner(Syntax & syntax, ActionFactory & /*action_factory*/)
{
  // add base formulation
  registerMooseObjectTask("add_mfem_formulation", MFEMFormulation, true);
  registerSyntaxTask("AddFormulationAction", "Formulation", "add_mfem_formulation");
  addTaskDependency("add_mfem_formulation", "init_mesh");
  addTaskDependency("add_variable", "add_mfem_formulation");
  addTaskDependency("add_aux_variable", "add_mfem_formulation");
  addTaskDependency("add_elemental_field_variable", "add_mfem_formulation");
  addTaskDependency("add_kernel", "add_mfem_formulation");

  // add coefficients
  registerMooseObjectTask("add_mfem_coefficients", MFEMCoefficient, false);
  registerSyntaxTask("AddCoefficientAction", "Coefficients/*", "add_mfem_coefficients");
  addTaskDependency("add_material", "add_mfem_coefficients");

  // add sources
  registerMooseObjectTask("add_mfem_sources", MFEMSource, false);
  registerSyntaxTask("AddSourceAction", "Sources/*", "add_mfem_sources");
  addTaskDependency("add_mfem_sources", "add_material");
  addTaskDependency("add_mfem_sources", "add_variable");
  addTaskDependency("add_mfem_sources", "add_aux_variable");

  // add FESpaces
  registerMooseObjectTask("add_mfem_fespaces", MFEMFESpace, false);
  registerSyntaxTask("AddFESpaceAction", "FESpaces/*", "add_mfem_fespaces");
  addTaskDependency("add_variable", "add_mfem_fespaces");
  addTaskDependency("add_aux_variable", "add_mfem_fespaces");
  addTaskDependency("add_elemental_field_variable", "add_mfem_fespaces");
  addTaskDependency("add_kernel", "add_mfem_fespaces");
  addTaskDependency("add_mfem_sources", "add_mfem_fespaces");
}

void
ApolloApp::registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  ModulesApp::registerAll(f, af, s);
  Registry::registerObjectsTo(f, {"ApolloApp"});
  Registry::registerActionsTo(af, {"ApolloApp"});
  /* register custom execute flags, action syntax, etc. here */
  associateSyntaxInner(s, af);
}

void
ApolloApp::registerApps()
{
  registerApp(ApolloApp);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
extern "C" void
ApolloApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  ApolloApp::registerAll(f, af, s);
}
extern "C" void
ApolloApp__registerApps()
{
  ApolloApp::registerApps();
}
