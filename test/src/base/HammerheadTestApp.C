//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
#include "HammerheadTestApp.h"
#include "HammerheadApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "MooseSyntax.h"
#include "ModulesApp.h"

InputParameters
HammerheadTestApp::validParams()
{
  InputParameters params = HammerheadApp::validParams();
  return params;
}

HammerheadTestApp::HammerheadTestApp(InputParameters parameters) : MooseApp(parameters)
{
  HammerheadTestApp::registerAll(
      _factory, _action_factory, _syntax, getParam<bool>("allow_test_objects"));
}

HammerheadTestApp::~HammerheadTestApp() {}

void
HammerheadTestApp::registerAll(Factory & f, ActionFactory & af, Syntax & s, bool use_test_objs)
{
  HammerheadApp::registerAll(f, af, s);
  if (use_test_objs)
  {
    Registry::registerObjectsTo(f, {"HammerheadTestApp"});
    Registry::registerActionsTo(af, {"HammerheadTestApp"});
  }
}

void
HammerheadTestApp::registerApps()
{
  registerApp(HammerheadApp);
  registerApp(HammerheadTestApp);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
// External entry point for dynamic application loading
extern "C" void
HammerheadTestApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  HammerheadTestApp::registerAll(f, af, s);
}
extern "C" void
HammerheadTestApp__registerApps()
{
  HammerheadTestApp::registerApps();
}
