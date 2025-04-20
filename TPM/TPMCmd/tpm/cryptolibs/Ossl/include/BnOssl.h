//** Introduction
// This file contains the headers necessary to build the Open SSL support for
// the TpmBigNum library.
#ifndef _BNOSSL_H_
#define _BNOSSL_H_
// TODO_RENAME_INC_FOLDER: public refers to the TPM_CoreLib public headers
#include "TPM/TPMCmd/tpm/include/public/tpm_public.h"
#include "TPM/TPMCmd/tpm/include/public/prototypes/TpmFail_fp.h"
#include "TPM/TPMCmd/tpm/cryptolibs/Ossl/include/Ossl/BnToOsslMath.h"
// TODO_RENAME_INC_FOLDER: these refer to TpmBigNum protected headers
#include "TPM/TPMCmd/tpm/cryptolibs/TpmBigNum/include/BnSupport_Interface.h"
#include "TPM/TPMCmd/tpm/cryptolibs/TpmBigNum/include/BnUtil_fp.h"
#include "TPM/TPMCmd/tpm/cryptolibs/TpmBigNum/include/BnMemory_fp.h"
#include "TPM/TPMCmd/tpm/cryptolibs/TpmBigNum/include/BnMath_fp.h"
#include "TPM/TPMCmd/tpm/cryptolibs/TpmBigNum/include/BnConvert_fp.h"

#if CRYPTO_LIB_REPORTING
#include "TPM/TPMCmd/tpm/cryptolibs/common/include/CryptoInterface.h"

//*** OsslGetVersion()
// Report the current version of OpenSSL.
void OsslGetVersion(_CRYPTO_IMPL_DESCRIPTION* result);

#endif  // CRYPTO_LIB_REPORTING

#endif  // _BNOSSL_H_
