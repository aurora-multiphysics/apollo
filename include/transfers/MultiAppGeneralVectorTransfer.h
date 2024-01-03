#pragma once

/**
 * Include headers of all MultiApp Transfers we would like.
 */
#include "MultiAppFieldTransfer.h"
#include "MultiAppTransfer.h"
#include "MultiAppCopyTransfer.h"
#include "MooseVariableFEBase.h"
#include "MultiAppGeneralFieldNearestLocationTransfer.h"
#include "AddVectorTransferAction.h"
#include "Factory.h"

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
  inline const AddVectorTransferAction & getAddVectorTransferAction() const
  {
    return *_add_vector_transfer_action;
  }

  void computeAuxKernels(const std::vector<VectorAuxKernel *> & vector_auxkernels) const;

private:
  const AddVectorTransferAction * _add_vector_transfer_action;
};