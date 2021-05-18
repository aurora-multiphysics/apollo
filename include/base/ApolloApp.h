#pragma once

#include "MooseApp.h"

class ApolloApp : public MooseApp
{
public:
  static InputParameters validParams();

  ApolloApp(InputParameters parameters);
  virtual ~ApolloApp();

  static void registerApps();
  static void registerAll(Factory & f, ActionFactory & af, Syntax & s);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
};

