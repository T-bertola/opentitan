
#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#include "TPM/TPMCmd/TpmConfiguration/TpmConfiguration/TpmBuildSwitches.h"
#include "TPM/TPMCmd/TpmConfiguration/TpmConfiguration/TpmProfile.h"
// TODO_RENAME_INC_FOLDER: public refers to the TPM_CoreLib public headers
#include "TPM/TPMCmd/tpm/include/public/BaseTypes.h"
#include "TPM/TPMCmd/tpm/include/public/TPMB.h"
#include "TPM/TPMCmd/tpm/include/public/MinMax.h"

#include "TPM/TPMCmd/Platform/include/PlatformACT.h"
#include "TPM/TPMCmd/Platform/include/PlatformClock.h"
#include "TPM/TPMCmd/Platform/include/PlatformData.h"
#include "TPM/TPMCmd/Platform/include/prototypes/platform_public_interface.h"
// TODO_RENAME_INC_FOLDER:platform_interface refers to the TPM_CoreLib platform interface
#include "TPM/TPMCmd/tpm/include/platform_interface/tpm_to_platform_interface.h"
#include "TPM/TPMCmd/tpm/include/platform_interface/platform_to_tpm_interface.h"

#define GLOBAL_C
#define NV_C
#include "TPM/TPMCmd/tpm/include/platform_interface/pcrstruct.h"
#include "TPM/TPMCmd/tpm/include/platform_interface/prototypes/platform_pcr_fp.h"

#endif  // _PLATFORM_H_
