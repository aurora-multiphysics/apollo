#include "MultiAppGeneralVectorTransfer.h"
#include "AuxiliarySystem.h"

#include "MultiAppGeneralFieldNearestLocationTransfer.h"
#include "MultiAppCopyTransfer.h"

#include "ApolloVectorTransferFlags.h"

/**
 * Register all Moose objects that we would like here.
 */
registerMooseObject("MooseApp", MultiAppGeneralFieldNearestLocationTransferVector);
registerMooseObject("MooseApp", MultiAppCopyTransferVector);

template <typename MultiAppTransferClassType>
InputParameters
MultiAppVectorTransferTemplate<MultiAppTransferClassType>::validParams()
{
  InputParameters params = MultiAppTransferClassType::validParams();

  params.addRequiredParam<AuxiliarySystem *>(
      "from_aux_system",
      "The auxiliary system for the problem we are transferrring variables from.");
  params.addRequiredParam<AuxiliarySystem *>(
      "to_aux_system", "The auxiliary system for the problem we are transferring variables to.");

  // Combine class description for original class with template class.
  params.addClassDescription(params.getClassDescription() + " (allows vector variables).");

  return params;
}

template <typename MultiAppTransferClassType>
MultiAppVectorTransferTemplate<MultiAppTransferClassType>::MultiAppVectorTransferTemplate(
    const InputParameters & parameters)
  : MultiAppTransferClassType(parameters),
    _from_aux_system(*parameters.get<AuxiliarySystem *>("from_aux_system")),
    _to_aux_system(*parameters.get<AuxiliarySystem *>("to_aux_system"))
{
}

template <typename MultiAppTransferClassType>
void
MultiAppVectorTransferTemplate<MultiAppTransferClassType>::execute()
{
  // Update all components of the vector variables.
  getFromAuxSystem().compute(ApolloApp::EXEC_PREPARE_VECTOR_FOR_TRANSFER);

  // Call execute on inherited class which will work on component variables.
  MultiAppTransferClassType::execute();

  // Rebuild all vector variables from their components.
  getToAuxSystem().compute(ApolloApp::EXEC_RECOVER_VECTOR_POST_TRANSFER);
}
