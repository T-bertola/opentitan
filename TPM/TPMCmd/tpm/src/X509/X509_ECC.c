//** Includes
#include "TPM/TPMCmd/tpm/include/private/Tpm.h"
#include "TPM/TPMCmd/tpm/include/private/X509.h"
#include "TPM/TPMCmd/tpm/include/private/OIDs.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/TpmASN1_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/X509_ECC_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/X509_spt_fp.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/CryptHash_fp.h"

//** Functions

//*** X509PushPoint()
// This seems like it might be used more than once so...
//  Return Type: INT16
//      > 0         number of bytes added
//     == 0         failure
INT16
X509PushPoint(ASN1MarshalContext* ctx, TPMS_ECC_POINT* p)
{
    // Push a bit string containing the public key. For now, push the x, and y
    // coordinates of the public point, bottom up
    ASN1StartMarshalContext(ctx);  // BIT STRING
    {
        ASN1PushBytes(ctx, p->y.t.size, p->y.t.buffer);
        ASN1PushBytes(ctx, p->x.t.size, p->x.t.buffer);
        ASN1PushByte(ctx, 0x04);
    }
    return ASN1EndEncapsulation(ctx, ASN1_BITSTRING);  // Ends BIT STRING
}

//*** X509AddSigningAlgorithmECC()
// This creates the singing algorithm data.
//  Return Type: INT16
//      > 0         number of bytes added
//     == 0         failure
INT16
X509AddSigningAlgorithmECC(
    OBJECT* signKey, TPMT_SIG_SCHEME* scheme, ASN1MarshalContext* ctx)
{
    PHASH_DEF hashDef = CryptGetHashDef(scheme->details.any.hashAlg);
    //
    NOT_REFERENCED(signKey);
    // If the desired hashAlg definition wasn't found...
    if(hashDef->hashAlg != scheme->details.any.hashAlg)
        return 0;

    switch(scheme->scheme)
    {
#if ALG_ECDSA
        case TPM_ALG_ECDSA:
            // Make sure that we have an OID for this hash and ECC
            if((hashDef->ECDSA)[0] != ASN1_OBJECT_IDENTIFIER)
                break;
            // if this is just an implementation check, indicate that this
            // combination is supported
            if(!ctx)
                return 1;
            ASN1StartMarshalContext(ctx);
            ASN1PushOID(ctx, hashDef->ECDSA);
            return ASN1EndEncapsulation(ctx, ASN1_CONSTRUCTED_SEQUENCE);
#endif  //  ALG_ECDSA
        default:
            break;
    }
    return 0;
}

//*** X509AddPublicECC()
// This function will add the publicKey description to the DER data. If ctx is
// NULL, then no data is transferred and this function will indicate if the TPM
// has the values for DER-encoding of the public key.
//  Return Type: INT16
//      > 0         number of bytes added
//     == 0         failure
INT16
X509AddPublicECC(OBJECT* object, ASN1MarshalContext* ctx)
{
    return 0;
}
