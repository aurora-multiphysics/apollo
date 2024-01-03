#include "MultiAppGeneralVectorTransfer.h" // Remove unused headers.
#include "FEProblemBase.h"
#include "SystemBase.h"
#include "VectorVariableFromComponentsAux.h"
#include "VectorVariableToComponentsAux.h"
#include "Moose.h"
#include "AuxiliarySystem.h"
#include "MooseObject.h"

/**
 * Register all Moose objects that we would like here.
 */
registerMooseObject("MooseApp", MultiAppGeneralFieldNearestLocationTransferVector);

template <typename MultiAppTransferClassType>
InputParameters
MultiAppVectorTransferTemplate<MultiAppTransferClassType>::validParams()
{
  InputParameters params = MultiAppTransferClassType::validParams();

  params.addRequiredParam<AddVectorTransferAction *>("add_vector_transfer_action",
                                                     "Pointer to vector transfer action class.");

  // Combine class description for original class with template class.
  params.addClassDescription(params.getClassDescription() + " (allows vector variables).");

  return params;
}

template <typename MultiAppTransferClassType>
MultiAppVectorTransferTemplate<MultiAppTransferClassType>::MultiAppVectorTransferTemplate(
    const InputParameters & parameters)
  : MultiAppTransferClassType(parameters),
    _add_vector_transfer_action(
        parameters.get<AddVectorTransferAction *>("add_vector_transfer_action"))
{
}

template <typename MultiAppTransferClassType>
void
MultiAppVectorTransferTemplate<MultiAppTransferClassType>::execute()
{
  // Run through the pre-transfer vector aux-kernels and call the compute() methods. This will
  // ensure that vector components are updated pre-transfer.
  computeAuxKernels(getAddVectorTransferAction().getPreTransferAuxKernels());

  // Call execute on inherited class which will work on standard variables.
  MultiAppTransferClassType::execute();

  // Run through the post-transfer vector aux-kernels and call the compute() methods. This will
  // rebuild the vectors from their components.
  computeAuxKernels(getAddVectorTransferAction().getPostTransferAuxKernels());
}

template <typename MultiAppTransferClassType>
void
MultiAppVectorTransferTemplate<MultiAppTransferClassType>::computeAuxKernels(
    const std::vector<VectorAuxKernel *> & vector_auxkernels) const
{
  for (auto * vector_aux_kernel_ptr : vector_auxkernels)
  {
    std::cout << "Now executing auxkernel " << vector_aux_kernel_ptr->name() << std::endl;

    if (vector_aux_kernel_ptr)
      vector_aux_kernel_ptr->compute();
  }
}