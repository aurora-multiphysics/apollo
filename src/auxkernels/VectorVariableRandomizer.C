#include "VectorVariableRandomizer.h"
#include <random>

registerMooseObject("MooseApp", VectorVariableRandomizer);

InputParameters
VectorVariableRandomizer::validParams()
{
  InputParameters params = WritableVectorAuxKernel::validParams();

  params.addClassDescription("Sets a vector variable to random values.");

  return params;
}

VectorVariableRandomizer::VectorVariableRandomizer(const InputParameters & parameters)
  : WritableVectorAuxKernel(parameters)
{
}

void
VectorVariableRandomizer::compute()
{
  std::default_random_engine generator;
  std::uniform_int_distribution<int> distribution(10, 1000);

  Real x = distribution(generator);
  Real y = distribution(generator);
  Real z = distribution(generator);

  _variable->setDofValue(x, 0);
  _variable->setDofValue(y, 1);
  _variable->setDofValue(z, 2);
}
