#include "BlockAveragePostprocessor.h"

// MOOSE includes
#include "MooseVariableScalar.h"
#include "SubProblem.h"

#include "libmesh/dof_map.h"

registerMooseObject("ApolloApp", BlockAveragePostprocessor);

InputParameters
BlockAveragePostprocessor::validParams()
{
  InputParameters params = GeneralPostprocessor::validParams();
  params.addParam<unsigned int>("block", 0, "Block to average");
  params.addRequiredParam<UserObjectName>(
      "block_average_userobject",
      "The name of the UserObject that is going to be computing the "
      "average value of a variable on each block");
  return params;
}

BlockAveragePostprocessor::BlockAveragePostprocessor(const InputParameters & parameters)
  : GeneralPostprocessor(parameters),
    _block(getParam<unsigned int>("block")),
    // When getting a UserObject from the input file pass the name
    // of the UserObjectName _parameter_
    // Note that getUserObject returns a _const reference_ of the type in < >
    _block_average_value(getUserObject<BlockAverageValue>("block_average_userobject"))

{
}

void
BlockAveragePostprocessor::initialize()
{
}

void
BlockAveragePostprocessor::execute()
{
}

Real
BlockAveragePostprocessor::getValue()
{

  return _block_average_value.averageValue(_block);
}
