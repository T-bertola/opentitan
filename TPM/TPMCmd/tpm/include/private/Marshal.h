
//** Introduction
// This file is used to provide the things needed by a module that uses the marshaling
// functions. It handles the variations between the marshaling choices (procedural or
// table-driven).
#include "TPM/TPMCmd/tpm/include/private/prototypes/Marshal_fp.h"

#if TABLE_DRIVEN_MARSHAL

#include "TPM/TPMCmd/tpm/include/private/TableMarshalTypes.h"

#include "TPM/TPMCmd/tpm/include/private/TableMarshalDefines.h"

#include "TPM/TPMCmd/tpm/include/private/prototypes/TableDrivenMarshal_fp.h"

#else

#include "TPM/TPMCmd/tpm/include/private/prototypes/Marshal_fp.h"

#endif
