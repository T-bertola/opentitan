// Root header file for building any TPM.lib code

#ifndef _TPM_H_
#define _TPM_H_

#include "sw/device/lib/runtime/log.h"
// TODO_RENAME_INC_FOLDER: public refers to the TPM_CoreLib public headers
#include "TPM/TPMCmd/tpm/include/public/tpm_public.h"

#include "TPM/TPMCmd/tpm/include/public/TpmAlgorithmDefines.h"
#include "TPM/TPMCmd/tpm/include/private/LibSupport.h"        // Types from the library. These need to come before
                               // Global.h because some of the structures in
                               // that file depend on the structures used by the
                               // cryptographic libraries.
#include "TPM/TPMCmd/tpm/include/public/GpMacros.h"          // Define additional macros
#include "TPM/TPMCmd/tpm/include/private/Global.h"            // Define other TPM types
#include "TPM/TPMCmd/tpm/include/private/InternalRoutines.h"  // Function prototypes

#include "sw/device/lib/base/memory.h"
#include "sw/device/lib/crypto/drivers/otbn.h"
#include "sw/device/lib/crypto/impl/rsa/rsa_datatypes.h"
#include "sw/device/lib/crypto/include/datatypes.h"
#include "sw/device/lib/crypto/include/hash.h"
#include "sw/device/lib/crypto/include/rsa.h"
#include "sw/device/lib/runtime/log.h"
#include "sw/device/lib/testing/entropy_testutils.h"
#include "sw/device/lib/testing/test_framework/check.h"
#include "sw/device/lib/testing/test_framework/ottf_main.h"


#endif  // _TPM_H_
