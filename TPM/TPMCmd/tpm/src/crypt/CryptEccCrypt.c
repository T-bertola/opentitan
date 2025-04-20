//** Includes and Defines
#include "TPM/TPMCmd/tpm/include/private/Tpm.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/TpmMath_Util_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/TpmEcc_Util_fp.h"

#if CC_ECC_Encrypt || CC_ECC_Encrypt

//** Functions

//*** CryptEccSelectScheme()
// This function is used by TPM2_ECC_Decrypt and TPM2_ECC_Encrypt.  It sets scheme
// either the input scheme or the key scheme. If they key scheme is not TPM_ALG_NULL
// then the input scheme must be TPM_ALG_NULL or the same as the key scheme. If
// not, then the function returns FALSE.
//  Return Type: BOOL
//      TRUE        'scheme' is set
//      FALSE       'scheme' is not valid (it may have been changed).
BOOL CryptEccSelectScheme(OBJECT*          key,    //IN: key containing default scheme
                          TPMT_KDF_SCHEME* scheme  // IN: a decrypt scheme
)
{
    TPMT_KDF_SCHEME* keyScheme = &key->publicArea.parameters.eccDetail.kdf;

    // Get sign object pointer
    if(scheme->scheme == TPM_ALG_NULL)
        *scheme = *keyScheme;
    if(keyScheme->scheme == TPM_ALG_NULL)
        keyScheme = scheme;
    return (
        scheme->scheme != TPM_ALG_NULL
        && (keyScheme->scheme == scheme->scheme
            && keyScheme->details.anyKdf.hashAlg == scheme->details.anyKdf.hashAlg));
}

//*** CryptEccEncrypt()
//This function performs ECC-based data obfuscation. The only scheme that is currently
// supported is MGF1 based. See Part 1, Annex D for details.
//  Return Type: TPM_RC
//      TPM_RC_CURVE            unsupported curve
//      TPM_RC_HASH             hash not allowed
//      TPM_RC_SCHEME           'scheme' is not supported
//      TPM_RC_NO_RESULT        internal error in big number processing
LIB_EXPORT TPM_RC CryptEccEncrypt(
    OBJECT*           key,        // IN: public key of recipient
    TPMT_KDF_SCHEME*  scheme,     // IN: scheme to use.
    TPM2B_MAX_BUFFER* plainText,  // IN: the text to obfuscate
    TPMS_ECC_POINT*   c1,         // OUT: public ephemeral key
    TPM2B_MAX_BUFFER* c2,         // OUT: obfuscated text
    TPM2B_DIGEST*     c3          // OUT: digest of ephemeral key
                                  //      and plainText
)
{
    LOG_INFO("IN CryptECCEncrypt");
    TPM_RC       retVal = TPM_RC_SUCCESS;
    return retVal;
}

//*** CryptEccDecrypt()
// This function performs ECC decryption and integrity check of the input data.
//  Return Type: TPM_RC
//      TPM_RC_CURVE            unsupported curve
//      TPM_RC_HASH             hash not allowed
//      TPM_RC_SCHEME           'scheme' is not supported
//      TPM_RC_NO_RESULT        internal error in big number processing
//      TPM_RC_VALUE            C3 did not match hash of recovered data
LIB_EXPORT TPM_RC CryptEccDecrypt(
    OBJECT*           key,        // IN: key used for data recovery
    TPMT_KDF_SCHEME*  scheme,     // IN: scheme to use.
    TPM2B_MAX_BUFFER* plainText,  // OUT: the recovered text
    TPMS_ECC_POINT*   c1,         // IN: public ephemeral key
    TPM2B_MAX_BUFFER* c2,         // IN: obfuscated text
    TPM2B_DIGEST*     c3          // IN: digest of ephemeral key
                                  //      and plainText
)
{
    LOG_INFO("CryptECCDecrypt");
    TPM_RC       retVal = TPM_RC_SUCCESS;
    return retVal;
}

#endif  // CC_ECC_Encrypt || CC_ECC_Encrypt
