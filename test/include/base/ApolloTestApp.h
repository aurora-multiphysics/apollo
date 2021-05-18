#pragma once

#include "MooseApp.h"

class ApolloTestApp : public MooseApp
{
public:
  static InputParameters validParams();

  ApolloTestApp(InputParameters parameters);
  virtual ~ApolloTestApp();

  static void registerApps();
  static void registerAll(Factory & f, ActionFactory & af, Syntax & s, bool use_test_objs = false);
};

