//** Includes and Defines
#include "TPM/TPMCmd/tpm/include/private/Tpm.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/TpmEcc_Signature_ECDSA_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/TpmEcc_Signature_ECDAA_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/TpmEcc_Signature_Schnorr_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/TpmEcc_Signature_SM2_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/TpmEcc_Util_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/TpmMath_Util_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/CryptEccSignature_fp.h"

#if ALG_ECC

//** Utility Functions

//** Signing Functions

//*** CryptEccSign()
// This function is the dispatch function for the various ECC-based
// signing schemes.
// There is a bit of ugliness to the parameter passing. In order to test this,
// we sometime would like to use a deterministic RNG so that we can get the same
// signatures during testing. The easiest way to do this for most schemes is to
// pass in a deterministic RNG and let it return canned values during testing.
// There is a competing need for a canned parameter to use in ECDAA. To accommodate
// both needs with minimal fuss, a special type of RAND_STATE is defined to carry
// the address of the commit value. The setup and handling of this is not very
// different for the caller than what was in previous versions of the code.
//  Return Type: TPM_RC
//      TPM_RC_SCHEME            'scheme' is not supported
LIB_EXPORT TPM_RC CryptEccSign(TPMT_SIGNATURE* signature,  // OUT: signature
                               OBJECT* signKey,  // IN: ECC key to sign the hash
                               const TPM2B_DIGEST* digest,  // IN: digest to sign
                               TPMT_ECC_SCHEME*    scheme,  // IN: signing scheme
                               RAND_STATE*         rand)
{
    TPM_RC           retVal = TPM_RC_SUCCESS;
    return retVal;
}

//********************* Signature Validation   ********************

//*** CryptEccValidateSignature()
// This function validates an EcDsa or EcSchnorr signature.
// The point 'Qin' needs to have been validated to be on the curve of 'curveId'.
//  Return Type: TPM_RC
//      TPM_RC_SIGNATURE            not a valid signature
LIB_EXPORT TPM_RC CryptEccValidateSignature(
    TPMT_SIGNATURE*     signature,  // IN: signature to be verified
    OBJECT*             signKey,    // IN: ECC key signed the hash
    const TPM2B_DIGEST* digest      // IN: digest that was signed
)
{

   TPM_RC           retVal = TPM_RC_SUCCESS;
   return retVal;
}

//***CryptEccCommitCompute()
// This function performs the point multiply operations required by TPM2_Commit.
//
// If 'B' or 'M' is provided, they must be on the curve defined by 'curveId'. This
// routine does not check that they are on the curve and results are unpredictable
// if they are not.
//
// It is a fatal error if 'r' is NULL. If 'B' is not NULL, then it is a
// fatal error if 'd' is NULL or if 'K' and 'L' are both NULL.
// If 'M' is not NULL, then it is a fatal error if 'E' is NULL.
//
//  Return Type: TPM_RC
//      TPM_RC_NO_RESULT        if 'K', 'L' or 'E' was computed to be the point
//                              at infinity
//      TPM_RC_CANCELED         a cancel indication was asserted during this
//                              function
LIB_EXPORT TPM_RC CryptEccCommitCompute(
    TPMS_ECC_POINT*      K,        // OUT: [d]B or [r]Q
    TPMS_ECC_POINT*      L,        // OUT: [r]B
    TPMS_ECC_POINT*      E,        // OUT: [r]M
    TPM_ECC_CURVE        curveId,  // IN: the curve for the computations
    TPMS_ECC_POINT*      M,        // IN: M (optional)
    TPMS_ECC_POINT*      B,        // IN: B (optional)
    TPM2B_ECC_PARAMETER* d,        // IN: d (optional)
    TPM2B_ECC_PARAMETER* r         // IN: the computed r value (required)
)
{
//     // Normally initialize E as the curve, but
//     // E means something else in this function
//     CRYPT_CURVE_INITIALIZED(curve, curveId);
//     CRYPT_ECC_INITIALIZED(bnR, r);
    LOG_INFO("In CryptEccCommitCompute");
       TPM_RC retVal = TPM_RC_SUCCESS;
//     //
//     // Validate that the required parameters are provided.
//     // Note: E has to be provided if computing E := [r]Q or E := [r]M. Will do
//     // E := [r]Q if both M and B are NULL.
//     pAssert(r != NULL && E != NULL);

//     // Initialize the output points in case they are not computed
//     ClearPoint2B(K);
//     ClearPoint2B(L);
//     ClearPoint2B(E);

//     // Sizes of the r parameter may not be zero
//     pAssert(r->t.size > 0);

//     // If B is provided, compute K=[d]B and L=[r]B
//     if(B != NULL)
//     {
//         CRYPT_ECC_INITIALIZED(bnD, d);
//         CRYPT_POINT_INITIALIZED(pB, B);
//         CRYPT_POINT_VAR(pK);
//         CRYPT_POINT_VAR(pL);
//         //
//         pAssert(d != NULL && K != NULL && L != NULL);

//         if(!ExtEcc_IsPointOnCurve(pB, curve))
//             ERROR_EXIT(TPM_RC_VALUE);
//         // do the math for K = [d]B
//         if((retVal = TpmEcc_PointMult(pK, pB, bnD, NULL, NULL, curve))
//            != TPM_RC_SUCCESS)
//             goto Exit;
//         // Convert BN K to TPM2B K
//         TpmEcc_PointTo2B(K, pK, curve);
//         //  compute L= [r]B after checking for cancel
//         if(_plat__IsCanceled())
//             ERROR_EXIT(TPM_RC_CANCELED);
//         // compute L = [r]B
//         if(!TpmEcc_IsValidPrivateEcc(bnR, curve))
//             ERROR_EXIT(TPM_RC_VALUE);
//         if((retVal = TpmEcc_PointMult(pL, pB, bnR, NULL, NULL, curve))
//            != TPM_RC_SUCCESS)
//             goto Exit;
//         // Convert BN L to TPM2B L
//         TpmEcc_PointTo2B(L, pL, curve);
//     }
//     if((M != NULL) || (B == NULL))
//     {
//         CRYPT_POINT_INITIALIZED(pM, M);
//         CRYPT_POINT_VAR(pE);
//         //
//         // Make sure that a place was provided for the result
//         pAssert(E != NULL);

//         // if this is the third point multiply, check for cancel first
//         if((B != NULL) && _plat__IsCanceled())
//             ERROR_EXIT(TPM_RC_CANCELED);

//         // If M provided, then pM will not be NULL and will compute E = [r]M.
//         // However, if M was not provided, then pM will be NULL and E = [r]G
//         // will be computed
//         if((retVal = TpmEcc_PointMult(pE, pM, bnR, NULL, NULL, curve))
//            != TPM_RC_SUCCESS)
//             goto Exit;
//         // Convert E to 2B format
//         TpmEcc_PointTo2B(E, pE, curve);
//     }
// Exit:
//     CRYPT_CURVE_FREE(curve);
    return retVal;
}

#endif  // ALG_ECC