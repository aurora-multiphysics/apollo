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
  // params.set<bool>("use_legacy_uo_initialization") = false;
  // params.set<bool>("use_legacy_uo_aux_computation") = false;
  // params.set<bool>("use_legacy_output_syntax") = false;
  // Do not use legacy DirichletBC, that is, set DirichletBC default for preset = true
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
  // Create the syntax
  registerSyntax("CMAction", "Modules/ComplexMaxwell");
  registerSyntax("AVAction", "Modules/AVFormulation");

  // add coefficients
  registerMooseObjectTask("add_mfem_coefficients", MFEMCoefficient, false);
  registerSyntaxTask("AddCoefficientAction", "Coefficients/*", "add_mfem_coefficients");
  addTaskDependency("add_material", "add_mfem_coefficients");

  // add variables action
  registerTask("add_maxwell_variables", /*is_required=*/false);
  addTaskDependency("add_maxwell_variables", "add_variable");

  // add ICs action
  registerTask("add_maxwell_ics", /*is_required=*/false);
  addTaskDependency("add_maxwell_ics", "add_ic");

  // add Kernels action
  registerTask("add_maxwell_kernels", /*is_required=*/false);
  addTaskDependency("add_maxwell_kernels", "add_kernel");

  // add BCs actions
  registerTask("add_maxwell_bcs", /*is_required=*/false);
  addTaskDependency("add_maxwell_bcs", "add_bc");
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
