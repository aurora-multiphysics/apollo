#include "HammerheadApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"
#include "MooseSyntax.h"

InputParameters
HammerheadApp::validParams()
{
  InputParameters params = MooseApp::validParams();

  // Do not use legacy DirichletBC, that is, set DirichletBC default for preset = true
  params.set<bool>("use_legacy_dirichlet_bc") = false;

  return params;
}

HammerheadApp::HammerheadApp(InputParameters parameters) : MooseApp(parameters)
{
  HammerheadApp::registerAll(_factory, _action_factory, _syntax);
}

HammerheadApp::~HammerheadApp() {}

void
HammerheadApp::registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  ModulesApp::registerAll(f, af, s);
  Registry::registerObjectsTo(f, {"HammerheadApp"});
  Registry::registerActionsTo(af, {"HammerheadApp"});

  /* register custom execute flags, action syntax, etc. here */
}

void
HammerheadApp::registerApps()
{
  registerApp(HammerheadApp);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
extern "C" void
HammerheadApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  HammerheadApp::registerAll(f, af, s);
}
extern "C" void
HammerheadApp__registerApps()
{
  HammerheadApp::registerApps();
}
