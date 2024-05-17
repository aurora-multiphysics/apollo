#include "ApolloVectorTransferFlags.h"

#include "ExecFlagRegistry.h"

const ExecFlagType Apollo::EXEC_PREPARE_VECTOR_FOR_TRANSFER =
    registerExecFlag("PREPARE_VECTOR_FOR_TRANSFER");
const ExecFlagType Apollo::EXEC_RECOVER_VECTOR_POST_TRANSFER =
    registerExecFlag("RECOVER_VECTOR_POST_TRANSFER");