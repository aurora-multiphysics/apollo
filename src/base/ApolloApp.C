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
  params.set<bool>("use_legacy_dirichlet_bc") = false;

  return params;
}

ApolloApp::ApolloApp(InputParameters parameters) : MooseApp(parameters)
{
  ApolloApp::registerAll(_factory, _action_factory, _syntax);
}

ApolloApp::~ApolloApp() {}

// static void
// associateSyntaxInner(Syntax & syntax, ActionFactory & /*action_factory*/)
// {
//   registerSyntaxTask(
//       "AddWaveguidePropertiesAction", "Modules/WaveguideProperties/*", "add_waveguide_properties");
//   registerMooseObjectTask("add_waveguide_properties", WaveguideProperties, false);
//   syntax.addDependency("add_waveguide_properties", "init_displaced_problem");

//   // syntax.registerActionSyntax("AddWaveguidePropertiesInterrogatorAction",
//   //                             "FluidPropertiesInterrogator");
// }

void
ApolloApp::registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  ModulesApp::registerAll(f, af, s);
  Registry::registerObjectsTo(f, {"ApolloApp"});
  Registry::registerActionsTo(af, {"ApolloApp"});

  /* register custom execute flags, action syntax, etc. here */
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
