#ifndef INTERNAL_ROUTINES_H
#define INTERNAL_ROUTINES_H

#if !defined _LIB_SUPPORT_H_ && !defined _TPM_H_
#  error "Should not be called"
#endif

// DRTM functions
// TODO_RENAME_INC_FOLDER:platform_interface refers to the TPM_CoreLib platform interface
#include "TPM/TPMCmd/tpm/include/platform_interface/prototypes/_TPM_Hash_Start_fp.h"
#include "TPM/TPMCmd/tpm/include/platform_interface/prototypes/_TPM_Hash_Data_fp.h"
#include "TPM/TPMCmd/tpm/include/platform_interface/prototypes/_TPM_Hash_End_fp.h"

// Internal subsystem functions
#include "TPM/TPMCmd/tpm/include/private/prototypes/Object_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/Context_spt_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/Object_spt_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/Entity_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/Session_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/Hierarchy_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/NvReserved_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/NvDynamic_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/NV_spt_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/ACT_spt_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/PCR_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/DA_fp.h"
// TODO_RENAME_INC_FOLDER: public refers to the TPM_CoreLib public headers
#include "TPM/TPMCmd/tpm/include/public/prototypes/TpmFail_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/SessionProcess_fp.h"

// Internal support functions
#include "TPM/TPMCmd/tpm/include/private/prototypes/CommandCodeAttributes_fp.h"
#include "TPM/TPMCmd/tpm/include/private/Marshal.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/Time_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/Locality_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/PP_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/CommandAudit_fp.h"
// TODO_RENAME_INC_FOLDER:platform_interface refers to the TPM_CoreLib platform interface
#include "TPM/TPMCmd/tpm/include/platform_interface/prototypes/Manufacture_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/Handle_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/Power_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/Response_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/CommandDispatcher_fp.h"

#ifdef CC_AC_Send
#include "TPM/TPMCmd/tpm/include/private/prototypes/AC_spt_fp.h"
#endif  // CC_AC_Send

// Miscellaneous
#include "TPM/TPMCmd/tpm/include/private/prototypes/Bits_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/AlgorithmCap_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/PropertyCap_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/IoBuffers_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/Memory_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/ResponseCodeProcessing_fp.h"

// Asymmetric Support library Interface
// TODO_RENAME_INC_FOLDER: needs a component prefix
// Math interface must be included before other Crypt headers to define types
#include "TPM/TPMCmd/tpm/cryptolibs/common/include/MathLibraryInterface.h"

// Internal cryptographic functions
#include "TPM/TPMCmd/tpm/include/private/prototypes/Ticket_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/CryptUtil_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/CryptHash_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/CryptSym_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/CryptPrime_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/CryptRand_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/CryptSelfTest_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/MathOnByteBuffers_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/CryptSym_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/AlgorithmTests_fp.h"

#if ALG_RSA
#include "TPM/TPMCmd/tpm/include/private/prototypes/CryptRsa_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/CryptPrimeSieve_fp.h"
#endif

#if ALG_ECC
#include "TPM/TPMCmd/tpm/include/private/prototypes/CryptEccMain_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/CryptEccSignature_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/CryptEccKeyExchange_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/CryptEccCrypt_fp.h"
#endif

#if CC_MAC || CC_MAC_Start
#include "TPM/TPMCmd/tpm/include/private/prototypes/CryptSmac_fp.h"
#  if ALG_CMAC
#    include "TPM/TPMCmd/tpm/include/private/prototypes/CryptCmac_fp.h"
#  endif
#endif

// Linkage to platform functions
// TODO_RENAME_INC_FOLDER:platform_interface refers to the TPM_CoreLib platform interface
#include "TPM/TPMCmd/tpm/include/platform_interface/tpm_to_platform_interface.h"

#endif

