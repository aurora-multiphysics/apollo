#pragma once

/**
 * Include headers of all MultiApp Transfers we would like.
 */
#include "AuxKernel.h"
#include "AuxiliarySystem.h"

// Forwards declaration.
template <typename MultiAppTransferClassType>
class MultiAppVectorTransferTemplate;

class MultiAppGeneralFieldNearestLocationTransfer;
class MultiAppCopyTransfer; // NB: CopyTransfer can handle vector variables.

/**
 * Create all transfer types using template here. Don't forget to register them in .C file.
 */
typedef MultiAppVectorTransferTemplate<MultiAppGeneralFieldNearestLocationTransfer>
    MultiAppGeneralFieldNearestLocationTransferVector;
typedef MultiAppVectorTransferTemplate<MultiAppCopyTransfer> MultiAppCopyTransferVector;

template <typename MultiAppTransferClassType>
class MultiAppVectorTransferTemplate : public MultiAppTransferClassType
{
public:
  static InputParameters validParams();

  MultiAppVectorTransferTemplate(const InputParameters & parameters);

  void execute() override final;

protected:
  inline AuxiliarySystem & getFromAuxSystem() const { return _from_aux_system; }
  inline AuxiliarySystem & getToAuxSystem() const { return _to_aux_system; }

private:
  AuxiliarySystem & _from_aux_system;
  AuxiliarySystem & _to_aux_system;
};