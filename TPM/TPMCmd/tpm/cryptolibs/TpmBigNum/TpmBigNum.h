//** Introduction
// This file contains the headers necessary to build the tpm big num library.
// TODO_RENAME_INC_FOLDER: public refers to the TPM_CoreLib public headers
#include "../../include/public/tpm_public.h"
#include "TPM/TPMCmd/tpm/include/public/prototypes/TpmFail_fp.h"
// TODO_RENAME_INC_FOLDER: private refers to the TPM_CoreLib private(protected) headers
#include "TPM/TPMCmd/tpm/include/public/TpmAlgorithmDefines.h"
#include "TPM/TPMCmd/tpm/include/public/GpMacros.h"  // required for TpmFail_fp.h
#include "TPM/TPMCmd/tpm/include/public/Capabilities.h"
#include "TPM/TPMCmd/tpm/include/public/TpmTypes.h"  // requires capabilities & GpMacros
#include "TPM/TPMCmd/tpm/cryptolibs/TpmBigNum/include/TpmBigNum/TpmToTpmBigNumMath.h"
#include "TPM/TPMCmd/tpm/cryptolibs/TpmBigNum/include/BnSupport_Interface.h"
#include "TPM/TPMCmd/tpm/cryptolibs/TpmBigNum/include/BnConvert_fp.h"
#include "TPM/TPMCmd/tpm/cryptolibs/TpmBigNum/include/BnMemory_fp.h"
#include "TPM/TPMCmd/tpm/cryptolibs/TpmBigNum/include/BnMath_fp.h"
#include "TPM/TPMCmd/tpm/cryptolibs/TpmBigNum/include/BnUtil_fp.h"
#include "TPM/TPMCmd/tpm/cryptolibs/common/include/MathLibraryInterface.h"