// This header file is used to select the library code that gets included in the
// TPM build.

#ifndef _LIB_SUPPORT_H_
#define _LIB_SUPPORT_H_
// TODO_RENAME_INC_FOLDER: public refers to the TPM_CoreLib public headers
#include "TPM/TPMCmd/tpm/include/public/tpm_radix.h"

#define SYM_LIB openssl

#define HASH_LIB openssl

// Include the options for hashing and symmetric. Defer the load of the math package
// Until the bignum parameters are defined.
#ifndef SYM_LIB
#  error SYM_LIB required
#endif
#ifndef HASH_LIB
#  error HASH_LIB required
#endif

 #include "TPM/TPMCmd/tpm/cryptolibs/Ossl/include/Ossl/TpmToOsslSym.h"
 #include "TPM/TPMCmd/tpm/cryptolibs/Ossl/include/Ossl/TpmToOsslHash.h"

//TODO: was #undef MIN
//was #undef MAX

#endif  // _LIB_SUPPORT_H_
