#include "MultiAppGeneralVectorTransfer.h" // TODO: - Remove unused headers.
#include "FEProblemBase.h"
#include "SystemBase.h"
#include "Moose.h"
#include "AuxiliarySystem.h"
#include "MooseObject.h"

#include "MultiAppGeneralFieldNearestLocationTransfer.h"
#include "MultiAppNearestNodeTransfer.h"

#include "VectorVariableFromComponentsAux.h"
#include "VectorVariableToComponentsAux.h"

/**
 * Register all Moose objects that we would like here.
 */
registerMooseObject("MooseApp", MultiAppGeneralFieldNearestLocationTransferVector);
registerMooseObject("MooseApp", MultiAppNearestNodeTransferVector);

template <typename MultiAppTransferClassType>
InputParameters
MultiAppVectorTransferTemplate<MultiAppTransferClassType>::validParams()
{
  InputParameters params = MultiAppTransferClassType::validParams();

  params.addRequiredParam<std::vector<VectorAuxKernel *>>(
      "pre_transfer_auxkernels",
      "Vector auxkernels called to update each vector's component variables before a transfer.");

  params.addRequiredParam<std::vector<VectorAuxKernel *>>(
      "post_transfer_auxkernels",
      "Vector auxkernels called to update a vector from its component variables after a transfer.");

  // Combine class description for original class with template class.
  params.addClassDescription(params.getClassDescription() + " (allows vector variables).");

  return params;
}

template <typename MultiAppTransferClassType>
MultiAppVectorTransferTemplate<MultiAppTransferClassType>::MultiAppVectorTransferTemplate(
    const InputParameters & parameters)
  : MultiAppTransferClassType(parameters),
    _pre_transfer_auxkernels(
        parameters.get<std::vector<VectorAuxKernel *>>("pre_transfer_auxkernels")),
    _post_transfer_auxkernels(
        parameters.get<std::vector<VectorAuxKernel *>>("post_transfer_auxkernels"))
{
}

template <typename MultiAppTransferClassType>
void
MultiAppVectorTransferTemplate<MultiAppTransferClassType>::execute()
{
  // Run through the pre-transfer vector aux-kernels and call the compute() methods. This will
  // ensure that vector components are updated pre-transfer.
  computeAuxKernels(getPreTransferAuxKernels());

  // Call execute on inherited class which will work on standard variables.
  MultiAppTransferClassType::execute();

  // Run through the post-transfer vector aux-kernels and call the compute() methods. This will
  // rebuild the vectors from their components.
  computeAuxKernels(getPostTransferAuxKernels());
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