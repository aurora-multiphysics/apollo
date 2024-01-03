#pragma once

/**
 * Include headers of all MultiApp Transfers we would like.
 */
#include "MultiAppGeneralFieldNearestLocationTransfer.h"
#include "AuxKernel.h"

// Forwards declaration.
template <typename MultiAppTransferClassType>
class MultiAppVectorTransferTemplate;

/**
 * Create all transfer types using template here. Don't forget to register them in .C file.
 */
typedef MultiAppVectorTransferTemplate<MultiAppGeneralFieldNearestLocationTransfer>
    MultiAppGeneralFieldNearestLocationTransferVector;

template <typename MultiAppTransferClassType>
class MultiAppVectorTransferTemplate : public MultiAppTransferClassType
{
public:
  static InputParameters validParams();

  MultiAppVectorTransferTemplate(const InputParameters & parameters);

  void execute() override final;

protected:
  void computeAuxKernels(const std::vector<VectorAuxKernel *> & vector_auxkernels) const;

  inline const std::vector<VectorAuxKernel *> & getPreTransferAuxKernels() const
  {
    return _pre_transfer_auxkernels;
  }

  inline const std::vector<VectorAuxKernel *> & getPostTransferAuxKernels() const
  {
    return _post_transfer_auxkernels;
  }

private:
  const std::vector<VectorAuxKernel *> _pre_transfer_auxkernels;
  const std::vector<VectorAuxKernel *> _post_transfer_auxkernels;
};