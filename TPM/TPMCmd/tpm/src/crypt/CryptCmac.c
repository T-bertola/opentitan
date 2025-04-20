//** Introduction
//
// This file contains the implementation of the message authentication codes based
// on a symmetric block cipher. These functions only use the single block
// encryption functions of the selected symmetric cryptographic library.

//** Includes, Defines, and Typedefs
#define _CRYPT_HASH_C_
#include "TPM/TPMCmd/tpm/include/private/Tpm.h"
#include "TPM/TPMCmd/tpm/include/private/CryptSym.h"

#if ALG_CMAC

//** Functions

//*** CryptCmacStart()
// This is the function to start the CMAC sequence operation. It initializes the
// dispatch functions for the data and end operations for CMAC and initializes the
// parameters that are used for the processing of data, including the key, key size
// and block cipher algorithm.
UINT16
CryptCmacStart(
    SMAC_STATE* state, TPMU_PUBLIC_PARMS* keyParms, TPM_ALG_ID macAlg, TPM2B* key)
{
    TPM_RC               retVal = TPM_RC_SUCCESS;
    return retVal;
}

//*** CryptCmacData()
// This function is used to add data to the CMAC sequence computation. The function
// will XOR new data into the IV. If the buffer is full, and there is additional
// input data, the data is encrypted into the IV buffer, the new data is then
// XOR into the IV. When the data runs out, the function returns without encrypting
// even if the buffer is full. The last data block of a sequence will not be
// encrypted until the call to CryptCmacEnd(). This is to allow the proper subkey
// to be computed and applied before the last block is encrypted.
void CryptCmacData(SMAC_STATES* state, UINT32 size, const BYTE* buffer)
{
    return;
}

//*** CryptCmacEnd()
// This is the completion function for the CMAC. It does padding, if needed, and
// selects the subkey to be applied before the last block is encrypted.
UINT16
CryptCmacEnd(SMAC_STATES* state, UINT32 outSize, BYTE* outBuffer)
{
    return 0;
}
#endif
