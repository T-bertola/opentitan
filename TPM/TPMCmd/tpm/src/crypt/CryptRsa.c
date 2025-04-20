//** Introduction
//
// This file contains implementation of cryptographic primitives for RSA.
// Vendors may replace the implementation in this file with their own library
// functions.

//**  Includes
// Need this define to get the 'private' defines for this function
#define CRYPT_RSA_C
#include "TPM/TPMCmd/tpm/include/private/Tpm.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/TpmMath_Util_fp.h"
#include <openssl/bn.h>


#define LITTLE_ENDIAN_PACKING

#if ALG_RSA

static const unsigned char kTestMessage[] = "Test message.";
static const size_t kTestMessageLen = sizeof(kTestMessage) - 1;

// OAEP label for testing.
static const unsigned char kTestLabel[] = "Test label.";
static const size_t kTestLabelLen = sizeof(kTestLabel) - 1;

static const otcrypto_hash_mode_t kTestHashMode = kOtcryptoHashModeSha256;
static const size_t kTestHashModeDigestBytes = 256 / 8;

//**  Obligatory Initialization Functions

//*** CryptRsaInit()
// Function called at _TPM_Init().
BOOL CryptRsaInit(void)
{
    return TRUE;
}

//*** CryptRsaStartup()
// Function called at TPM2_Startup()
BOOL CryptRsaStartup(void)
{
    return TRUE;
}

//** Internal Functions

//*** RsaInitializeExponent()
// This function initializes the bignum data structure that holds the private
// exponent. This function returns the pointer to the private exponent value so that
// it can be used in an initializer for a data declaration.

static privateExponent* RsaInitializeExponent(privateExponent* Z)
{
    // verify privateExponent packing matches the usage of the bn pointer as an
    // array in below function
    MUST_BE(offsetof(privateExponent, Q) == SIZEOF_MEMBER(privateExponent, P));

    Crypt_Int** bn = (Crypt_Int**)&Z->P;
    int         i;
    //
    for(i = 0; i < 5; i++)
    {
        bn[i] = (Crypt_Int*)&(Z->entries[i]);
        ExtMath_Initialize_Int(bn[i], MAX_RSA_KEY_BITS / 2);
    }
    return Z;
}

//*** MakePgreaterThanQ()
// This function swaps the pointers for P and Q if Q happens to be larger than Q.
static void MakePgreaterThanQ(privateExponent* Z)
{
    if(ExtMath_UnsignedCmp(Z->P, Z->Q) < 0)
    {
        Crypt_Int* bnT = Z->P;
        Z->P           = Z->Q;
        Z->Q           = bnT;
    }
}

//*** PackExponent()
// This function takes the bignum private exponent and converts it into TPM2B form.
// In this form, the size field contains the overall size of the packed data. The
// buffer contains 5, equal sized values in P, Q, dP, dQ, qInv order. For example, if
// a key has a 2Kb public key, then the packed private key will contain 5, 1Kb values.
// This form makes it relatively easy to load and save the values without changing
// the normal unmarshaling to do anything more than allow a larger TPM2B for the
// private key. Also, when exporting the value, all that is needed is to change the
// size field of the private key in order to save just the P value.
//  Return Type: BOOL
//      TRUE(1)     success
//      FALSE(0)    failure         // The data is too big to fit
static BOOL PackExponent(TPM2B_PRIVATE_KEY_RSA* packed, privateExponent* Z)
{
    int    i;
    UINT16 primeSize = (UINT16)BITS_TO_BYTES(ExtMath_MostSigBitNum(Z->P));
    UINT16 pS        = primeSize;
    //
    pAssert((primeSize * 5) <= sizeof(packed->t.buffer));
    packed->t.size = (primeSize * 5) + RSA_prime_flag;
    for(i = 0; i < 5; i++)
        if(!ExtMath_IntToBytes(
               (Crypt_Int*)&Z->entries[i], &packed->t.buffer[primeSize * i], &pS))
            return FALSE;
    if(pS != primeSize)
        return FALSE;
    return TRUE;
}

//*** UnpackExponent()
// This function unpacks the private exponent from its TPM2B form into its bignum
// form.
//  Return Type: BOOL
//      TRUE(1)         success
//      FALSE(0)        TPM2B is not the correct size
static BOOL UnpackExponent(TPM2B_PRIVATE_KEY_RSA* b, privateExponent* Z)
{
    UINT16      primeSize = b->t.size & ~RSA_prime_flag;
    int         i;
    Crypt_Int** bn = &Z->P;
    //
    GOTO_ERROR_UNLESS(b->t.size & RSA_prime_flag);
    RsaInitializeExponent(Z);
    GOTO_ERROR_UNLESS((primeSize % 5) == 0);
    primeSize /= 5;
    for(i = 0; i < 5; i++)
        GOTO_ERROR_UNLESS(
            ExtMath_IntFromBytes(bn[i], &b->t.buffer[primeSize * i], primeSize)
            != NULL);
    MakePgreaterThanQ(Z);
    return TRUE;
Error:
    return FALSE;
}

//*** ComputePrivateExponent()
// This function computes the private exponent from the primes.
//  Return Type: BOOL
//      TRUE(1)         success
//      FALSE(0)        failure
static BOOL ComputePrivateExponent(
    Crypt_Int*       pubExp,  // IN: the public exponent
    privateExponent* Z        // IN/OUT: on input, has primes P and Q. On
                              //         output, has P, Q, dP, dQ, and pInv
)
{
    BOOL pOK;
    BOOL qOK;
    CRYPT_PRIME_VAR(pT);
    //
    // make p the larger value so that m2 is always less than p
    MakePgreaterThanQ(Z);

    //dP = (1/e) mod (p-1)
    pOK = ExtMath_SubtractWord(pT, Z->P, 1);
    pOK = pOK && ExtMath_ModInverse(Z->dP, pubExp, pT);
    //dQ = (1/e) mod (q-1)
    qOK = ExtMath_SubtractWord(pT, Z->Q, 1);
    qOK = qOK && ExtMath_ModInverse(Z->dQ, pubExp, pT);
    // qInv = (1/q) mod p
    if(pOK && qOK)
        pOK = qOK = ExtMath_ModInverse(Z->qInv, Z->Q, Z->P);
    if(!pOK)
        ExtMath_SetWord(Z->P, 0);
    if(!qOK)
        ExtMath_SetWord(Z->Q, 0);
    return pOK && qOK;
}

//*** RsaPrivateKeyOp()
// This function is called to do the exponentiation with the private key. Compile
// options allow use of the simple (but slow) private exponent, or the more complex
// but faster CRT method.
//  Return Type: BOOL
//      TRUE(1)         success
//      FALSE(0)        failure
static BOOL RsaPrivateKeyOp(Crypt_Int* inOut,  // IN/OUT: number to be exponentiated
                            privateExponent* Z)
{
    CRYPT_RSA_VAR(M1);
    CRYPT_RSA_VAR(M2);
    CRYPT_RSA_VAR(M);
    CRYPT_RSA_VAR(H);
    //
    MakePgreaterThanQ(Z);
    // m1 = cdP mod p
    GOTO_ERROR_UNLESS(ExtMath_ModExp(M1, inOut, Z->dP, Z->P));
    // m2 = cdQ mod q
    GOTO_ERROR_UNLESS(ExtMath_ModExp(M2, inOut, Z->dQ, Z->Q));
    // h = qInv * (m1 - m2) mod p = qInv * (m1 + P - m2) mod P because Q < P
    // so m2 < P
    GOTO_ERROR_UNLESS(ExtMath_Subtract(H, Z->P, M2));
    GOTO_ERROR_UNLESS(ExtMath_Add(H, H, M1));
    GOTO_ERROR_UNLESS(ExtMath_ModMult(H, H, Z->qInv, Z->P));
    // m = m2 + h * q
    GOTO_ERROR_UNLESS(ExtMath_Multiply(M, H, Z->Q));
    GOTO_ERROR_UNLESS(ExtMath_Add(inOut, M2, M));
    return TRUE;
Error:
    return FALSE;
}

//*** RSAEP()
// This function performs the RSAEP operation defined in PKCS#1v2.1. It is
// an exponentiation of a value ('m') with the public exponent ('e'), modulo
// the public ('n').
//
//  Return Type: TPM_RC
//      TPM_RC_VALUE     number to exponentiate is larger than the modulus
//
static TPM_RC RSAEP(TPM2B* dInOut,  // IN: size of the encrypted block and the size of
                                    //     the encrypted value. It must be the size of
                                    //     the modulus.
                                    // OUT: the encrypted data. Will receive the
                                    //      decrypted value
                    OBJECT* key     // IN: the key to use
)
{
    TPM2B_TYPE(4BYTES, 4);
    TPM2B_4BYTES e2B;
    UINT32       e = key->publicArea.parameters.rsaDetail.exponent;
    //
    if(e == 0)
        e = RSA_DEFAULT_PUBLIC_EXPONENT;
    UINT32_TO_BYTE_ARRAY(e, e2B.t.buffer);
    e2B.t.size = 4;
    return ModExpB(dInOut->size,
                   dInOut->buffer,
                   dInOut->size,
                   dInOut->buffer,
                   e2B.t.size,
                   e2B.t.buffer,
                   key->publicArea.unique.rsa.t.size,
                   key->publicArea.unique.rsa.t.buffer);
}

//*** RSADP()
// This function performs the RSADP operation defined in PKCS#1v2.1. It is
// an exponentiation of a value ('c') with the private exponent ('d'), modulo
// the public modulus ('n'). The decryption is in place.
//
// This function also checks the size of the private key. If the size indicates
// that only a prime value is present, the key is converted to being a private
// exponent.
//
//  Return Type: TPM_RC
//      TPM_RC_SIZE         the value to decrypt is larger than the modulus
//
static TPM_RC RSADP(TPM2B*  inOut,  // IN/OUT: the value to encrypt
                    OBJECT* key     // IN: the key
)
{
    CRYPT_RSA_INITIALIZED(bnM, inOut);
    NEW_PRIVATE_EXPONENT(Z);
    if(UnsignedCompareB(inOut->size,
                        inOut->buffer,
                        key->publicArea.unique.rsa.t.size,
                        key->publicArea.unique.rsa.t.buffer)
       >= 0)
        return TPM_RC_SIZE;
    // private key operation requires that private exponent be loaded
    // During self-test, this might not be the case so load it up if it hasn't
    // already done
    // been done
    if((key->sensitive.sensitive.rsa.t.size & RSA_prime_flag) == 0)
    {
        if(CryptRsaLoadPrivateExponent(&key->publicArea, &key->sensitive)
           != TPM_RC_SUCCESS)
            return TPM_RC_BINDING;
    }
    GOTO_ERROR_UNLESS(UnpackExponent(&key->sensitive.sensitive.rsa, Z));
    GOTO_ERROR_UNLESS(RsaPrivateKeyOp(bnM, Z));
    GOTO_ERROR_UNLESS(TpmMath_IntTo2B(bnM, inOut, inOut->size));
    return TPM_RC_SUCCESS;
Error:
    return TPM_RC_FAILURE;
}

//*** OaepEncode()
// This function performs OAEP padding. The size of the buffer to receive the
// OAEP padded data must equal the size of the modulus
//
//  Return Type: TPM_RC
//      TPM_RC_VALUE     'hashAlg' is not valid or message size is too large
//
static TPM_RC OaepEncode(
    TPM2B*       padded,   // OUT: the pad data
    TPM_ALG_ID   hashAlg,  // IN: algorithm to use for padding
    const TPM2B* label,    // IN: null-terminated string (may be NULL)
    TPM2B*       message,  // IN: the message being padded
    RAND_STATE*  rand      // IN: the random number generator to use
)
{
    INT32  padLen;
    INT32  dbSize;
    INT32  i;
    BYTE   mySeed[MAX_DIGEST_SIZE];
    BYTE*  seed = mySeed;
    UINT16 hLen = CryptHashGetDigestSize(hashAlg);
    BYTE   mask[MAX_RSA_KEY_BYTES];
    BYTE*  pp;
    BYTE*  pm;
    TPM_RC retVal = TPM_RC_SUCCESS;

    pAssert(padded != NULL && message != NULL);

    // A value of zero is not allowed because the KDF can't produce a result
    // if the digest size is zero.
    if(hLen == 0)
        return TPM_RC_VALUE;

    // Basic size checks
    //  make sure digest isn't too big for key size
    if(padded->size < (2 * hLen) + 2)
        ERROR_EXIT(TPM_RC_HASH);

    // and that message will fit messageSize <= k - 2hLen - 2
    if(message->size > (padded->size - (2 * hLen) - 2))
        ERROR_EXIT(TPM_RC_VALUE);

    // Hash L even if it is null
    // Offset into padded leaving room for masked seed and byte of zero
    pp = &padded->buffer[hLen + 1];
    if(CryptHashBlock(hashAlg, label->size, (BYTE*)label->buffer, hLen, pp) != hLen)
        ERROR_EXIT(TPM_RC_FAILURE);

    // concatenate PS of k  mLen  2hLen  2
    padLen = padded->size - message->size - (2 * hLen) - 2;
    MemorySet(&pp[hLen], 0, padLen);
    pp[hLen + padLen] = 0x01;
    padLen += 1;
    memcpy(&pp[hLen + padLen], message->buffer, message->size);

    // The total size of db = hLen + pad + mSize;
    dbSize = hLen + padLen + message->size;

    DRBG_Generate(rand, mySeed, (UINT16)hLen);
    if(g_inFailureMode)
        ERROR_EXIT(TPM_RC_FAILURE);
    // mask = MGF1 (seed, nSize  hLen  1)
    CryptMGF_KDF(dbSize, mask, hashAlg, hLen, seed, 0);

    // Create the masked db
    pm = mask;
    for(i = dbSize; i > 0; i--)
        *pp++ ^= *pm++;
    pp = &padded->buffer[hLen + 1];

    // Run the masked data through MGF1
    if(CryptMGF_KDF(hLen, &padded->buffer[1], hashAlg, dbSize, pp, 0)
       != (unsigned)hLen)
        ERROR_EXIT(TPM_RC_VALUE);
    // Now XOR the seed to create masked seed
    pp = &padded->buffer[1];
    pm = seed;
    for(i = hLen; i > 0; i--)
        *pp++ ^= *pm++;
    // Set the first byte to zero
    padded->buffer[0] = 0x00;
Exit:
    return retVal;
}

//*** OaepDecode()
// This function performs OAEP padding checking. The size of the buffer to receive
// the recovered data. If the padding is not valid, the 'dSize' size is set to zero
// and the function returns TPM_RC_VALUE.
//
// The 'dSize' parameter is used as an input to indicate the size available in the
// buffer.

// If insufficient space is available, the size is not changed and the return code
// is TPM_RC_VALUE.
//
//  Return Type: TPM_RC
//      TPM_RC_VALUE        the value to decode was larger than the modulus, or
//                          the padding is wrong or the buffer to receive the
//                          results is too small
//
//
static TPM_RC OaepDecode(
    TPM2B*       dataOut,  // OUT: the recovered data
    TPM_ALG_ID   hashAlg,  // IN: algorithm to use for padding
    const TPM2B* label,    // IN: null-terminated string (may be NULL)
    TPM2B*       padded    // IN: the padded data
)
{
    UINT32 i;
    BYTE   seedMask[MAX_DIGEST_SIZE];
    UINT32 hLen = CryptHashGetDigestSize(hashAlg);

    BYTE   mask[MAX_RSA_KEY_BYTES];
    BYTE*  pp;
    BYTE*  pm;
    TPM_RC retVal = TPM_RC_SUCCESS;

    // Strange size (anything smaller can't be an OAEP padded block)
    // Also check for no leading 0
    if((padded->size < (unsigned)((2 * hLen) + 2)) || (padded->buffer[0] != 0))
        ERROR_EXIT(TPM_RC_VALUE);
    // Use the hash size to determine what to put through MGF1 in order
    // to recover the seedMask
    CryptMGF_KDF(hLen,
                 seedMask,
                 hashAlg,
                 padded->size - hLen - 1,
                 &padded->buffer[hLen + 1],
                 0);

    // Recover the seed into seedMask
    pAssert(hLen <= sizeof(seedMask));
    pp = &padded->buffer[1];
    pm = seedMask;
    for(i = hLen; i > 0; i--)
        *pm++ ^= *pp++;

    // Use the seed to generate the data mask
    CryptMGF_KDF(padded->size - hLen - 1, mask, hashAlg, hLen, seedMask, 0);

    // Use the mask generated from seed to recover the padded data
    pp = &padded->buffer[hLen + 1];
    pm = mask;
    for(i = (padded->size - hLen - 1); i > 0; i--)
        *pm++ ^= *pp++;

    // Make sure that the recovered data has the hash of the label
    // Put trial value in the seed mask
    if((CryptHashBlock(hashAlg, label->size, (BYTE*)label->buffer, hLen, seedMask))
       != hLen)
        FAIL(FATAL_ERROR_INTERNAL);
    if(memcmp(seedMask, mask, hLen) != 0)
        ERROR_EXIT(TPM_RC_VALUE);

    // find the start of the data
    pm = &mask[hLen];
    for(i = (UINT32)padded->size - (2 * hLen) - 1; i > 0; i--)
    {
        if(*pm++ != 0)
            break;
    }
    // If we ran out of data or didn't end with 0x01, then return an error
    if(i == 0 || pm[-1] != 0x01)
        ERROR_EXIT(TPM_RC_VALUE);

    // pm should be pointing at the first part of the data
    // and i is one greater than the number of bytes to move
    i--;
    if(i > dataOut->size)
        // Special exit to preserve the size of the output buffer
        return TPM_RC_VALUE;
    memcpy(dataOut->buffer, pm, i);
    dataOut->size = (UINT16)i;
Exit:
    if(retVal != TPM_RC_SUCCESS)
        dataOut->size = 0;
    return retVal;
}

//*** PKCS1v1_5Encode()
// This function performs the encoding for RSAES-PKCS1-V1_5-ENCRYPT as defined in
// PKCS#1V2.1
//  Return Type: TPM_RC
//      TPM_RC_VALUE     message size is too large
//
static TPM_RC RSAES_PKCS1v1_5Encode(TPM2B* padded,   // OUT: the pad data
                                    TPM2B* message,  // IN: the message being padded
                                    RAND_STATE* rand)
{
    UINT32 ps = padded->size - message->size - 3;
    //
    if(message->size > padded->size - 11)
        return TPM_RC_VALUE;
    // move the message to the end of the buffer
    memcpy(&padded->buffer[padded->size - message->size],
           message->buffer,
           message->size);
    // Set the first byte to 0x00 and the second to 0x02
    padded->buffer[0] = 0;
    padded->buffer[1] = 2;

    // Fill with random bytes
    DRBG_Generate(rand, &padded->buffer[2], (UINT16)ps);
    if(g_inFailureMode)
        return TPM_RC_FAILURE;

    // Set the delimiter for the random field to 0
    padded->buffer[2 + ps] = 0;

    // Now, the only messy part. Make sure that all the 'ps' bytes are non-zero
    // In this implementation, use the value of the current index
    for(ps++; ps > 1; ps--)
    {
        if(padded->buffer[ps] == 0)
            padded->buffer[ps] = 0x55;  // In the < 0.5% of the cases that the
                                        // random value is 0, just pick a value to
                                        // put into the spot.
    }
    return TPM_RC_SUCCESS;
}

//*** RSAES_Decode()
// This function performs the decoding for RSAES-PKCS1-V1_5-ENCRYPT as defined in
// PKCS#1V2.1
//
//  Return Type: TPM_RC
//      TPM_RC_FAIL      decoding error or results would no fit into provided buffer
//
static TPM_RC RSAES_Decode(TPM2B* message,  // OUT: the recovered message
                           TPM2B* coded     // IN: the encoded message
)
{
    BOOL   fail = FALSE;
    UINT16 pSize;

    fail = (coded->size < 11);
    fail = (coded->buffer[0] != 0x00) | fail;
    fail = (coded->buffer[1] != 0x02) | fail;
    for(pSize = 2; pSize < coded->size; pSize++)
    {
        if(coded->buffer[pSize] == 0)
            break;
    }
    pSize++;

    // Make sure that pSize has not gone over the end and that there are at least 8
    // bytes of pad data.
    fail = (pSize > coded->size) | fail;
    fail = ((pSize - 2) <= 8) | fail;
    if((message->size < (UINT16)(coded->size - pSize)) || fail)
        return TPM_RC_VALUE;
    message->size = coded->size - pSize;
    memcpy(message->buffer, &coded->buffer[pSize], coded->size - pSize);
    return TPM_RC_SUCCESS;
}

//*** CryptRsaPssSaltSize()
// This function computes the salt size used in PSS. It is broken out so that
// the X509 code can get the same value that is used by the encoding function in this
// module.
INT16
CryptRsaPssSaltSize(INT16 hashSize, INT16 outSize)
{
    INT16 saltSize;
    //
    // (Mask Length) = (outSize - hashSize - 1);
    // Max saltSize is (Mask Length) - 1
    saltSize = (outSize - hashSize - 1) - 1;
    // Use the maximum salt size allowed by FIPS 186-4
    if(saltSize > hashSize)
        saltSize = hashSize;
    else if(saltSize < 0)
        saltSize = 0;
    return saltSize;
}

//*** PssEncode()
// This function creates an encoded block of data that is the size of modulus.
// The function uses the maximum salt size that will fit in the encoded block.
//
//  Returns TPM_RC_SUCCESS or goes into failure mode.
static TPM_RC PssEncode(TPM2B*      out,      // OUT: the encoded buffer
                        TPM_ALG_ID  hashAlg,  // IN: hash algorithm for the encoding
                        TPM2B*      digest,   // IN: the digest
                        RAND_STATE* rand      // IN: random number source
)
{
    UINT32     hLen = CryptHashGetDigestSize(hashAlg);
    BYTE       salt[MAX_RSA_KEY_BYTES - 1];
    UINT16     saltSize;
    BYTE*      ps = salt;
    BYTE*      pOut;
    UINT16     mLen;
    HASH_STATE hashState;

    // These are fatal errors indicating bad TPM firmware
    pAssert(out != NULL && hLen > 0 && digest != NULL);

    // Get the size of the mask
    mLen = (UINT16)(out->size - hLen - 1);

    // Set the salt size
    saltSize = CryptRsaPssSaltSize((INT16)hLen, (INT16)out->size);

    //using eOut for scratch space
    // Set the first 8 bytes to zero
    pOut = out->buffer;
    memset(pOut, 0, 8);

    // Get set the salt
    DRBG_Generate(rand, salt, saltSize);
    if(g_inFailureMode)
        return TPM_RC_FAILURE;

    // Create the hash of the pad || input hash || salt
    CryptHashStart(&hashState, hashAlg);
    CryptDigestUpdate(&hashState, 8, pOut);
    CryptDigestUpdate2B(&hashState, digest);
    CryptDigestUpdate(&hashState, saltSize, salt);
    CryptHashEnd(&hashState, hLen, &pOut[out->size - hLen - 1]);

    // Create a mask
    if(CryptMGF_KDF(mLen, pOut, hashAlg, hLen, &pOut[mLen], 0) != mLen)
        FAIL(FATAL_ERROR_INTERNAL);

    // Since this implementation uses key sizes that are all even multiples of
    // 8, just need to make sure that the most significant bit is CLEAR
    *pOut &= 0x7f;

    // Before we mess up the pOut value, set the last byte to 0xbc
    pOut[out->size - 1] = 0xbc;

    // XOR a byte of 0x01 at the position just before where the salt will be XOR'ed
    pOut = &pOut[mLen - saltSize - 1];
    *pOut++ ^= 0x01;

    // XOR the salt data into the buffer
    for(; saltSize > 0; saltSize--)
        *pOut++ ^= *ps++;

    // and we are done
    return TPM_RC_SUCCESS;
}

//*** PssDecode()
// This function checks that the PSS encoded block was built from the
// provided digest. If the check is successful, TPM_RC_SUCCESS is returned.
// Any other value indicates an error.
//
// This implementation of PSS decoding is intended for the reference TPM
// implementation and is not at all generalized.  It is used to check
// signatures over hashes and assumptions are made about the sizes of values.
// Those assumptions are enforce by this implementation.
// This implementation does allow for a variable size salt value to have been
// used by the creator of the signature.
//
//  Return Type: TPM_RC
//      TPM_RC_SCHEME       'hashAlg' is not a supported hash algorithm
//      TPM_RC_VALUE         decode operation failed
//
static TPM_RC PssDecode(
    TPM_ALG_ID hashAlg,  // IN: hash algorithm to use for the encoding
    TPM2B*     dIn,      // In: the digest to compare
    TPM2B*     eIn       // IN: the encoded data
)
{
    TPM_RC               retVal = TPM_RC_SUCCESS;
    return retVal;
}

//*** MakeDerTag()
// Construct the DER value that is used in RSASSA
//  Return Type: INT16
//   > 0        size of value
//   <= 0       no hash exists
INT16
MakeDerTag(TPM_ALG_ID hashAlg, INT16 sizeOfBuffer, BYTE* buffer)
{
    //    0x30, 0x31,       // SEQUENCE (2 elements) 1st
    //        0x30, 0x0D,   // SEQUENCE (2 elements)
    //            0x06, 0x09,   // HASH OID
    //                0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01,
    //             0x05, 0x00,  // NULL
    //        0x04, 0x20  //  OCTET STRING
    HASH_DEF* info = CryptGetHashDef(hashAlg);
    INT16     oidSize;
    // If no OID, can't do encode
    GOTO_ERROR_UNLESS(info != NULL);
    oidSize = 2 + (info->OID)[1];
    // make sure this fits in the buffer
    GOTO_ERROR_UNLESS(sizeOfBuffer >= (oidSize + 8));
    *buffer++ = 0x30;  // 1st SEQUENCE
    // Size of the 1st SEQUENCE is 6 bytes + size of the hash OID + size of the
    // digest size
    *buffer++ = (BYTE)(6 + oidSize + info->digestSize);  //
    *buffer++ = 0x30;                                    // 2nd SEQUENCE
    // size is 4 bytes of overhead plus the side of the OID
    *buffer++ = (BYTE)(2 + oidSize);
    MemoryCopy(buffer, info->OID, oidSize);
    buffer += oidSize;
    *buffer++ = 0x05;  // Add a NULL
    *buffer++ = 0x00;

    *buffer++ = 0x04;
    *buffer++ = (BYTE)(info->digestSize);
    return oidSize + 8;
Error:
    return 0;
}

//*** RSASSA_Encode()
// Encode a message using PKCS1v1.5 method.
//
//  Return Type: TPM_RC
//      TPM_RC_SCHEME       'hashAlg' is not a supported hash algorithm
//      TPM_RC_SIZE         'eOutSize' is not large enough
//      TPM_RC_VALUE        'hInSize' does not match the digest size of hashAlg
static TPM_RC RSASSA_Encode(TPM2B* pOut,  // IN:OUT on in, the size of the public key
                                          //        on out, the encoded area
                            TPM_ALG_ID hashAlg,  // IN: hash algorithm for PKCS1v1_5
                            TPM2B*     hIn       // IN: digest value to encode
)
{
    TPM_RC               retVal = TPM_RC_SUCCESS;
    return retVal;
}

//*** RSASSA_Decode()
// This function performs the RSASSA decoding of a signature.
//
//  Return Type: TPM_RC
//      TPM_RC_VALUE          decode unsuccessful
//      TPM_RC_SCHEME        'haslAlg' is not supported
//
static TPM_RC RSASSA_Decode(
    TPM_ALG_ID hashAlg,  // IN: hash algorithm to use for the encoding
    TPM2B*     hIn,      // In: the digest to compare
    TPM2B*     eIn       // IN: the encoded data
)
{
    TPM_RC               retVal = TPM_RC_SUCCESS;
    return retVal;
}

//** Externally Accessible Functions

//*** CryptRsaSelectScheme()
// This function is used by TPM2_RSA_Decrypt and TPM2_RSA_Encrypt.  It sets up
// the rules to select a scheme between input and object default.
// This function assume the RSA object is loaded.
// If a default scheme is defined in object, the default scheme should be chosen,
// otherwise, the input scheme should be chosen.
// In the case that both the object and 'scheme' are not TPM_ALG_NULL, then
// if the schemes are the same, the input scheme will be chosen.
// if the scheme are not compatible, a NULL pointer will be returned.
//
// The return pointer may point to a TPM_ALG_NULL scheme.
TPMT_RSA_DECRYPT* CryptRsaSelectScheme(
    TPMI_DH_OBJECT    rsaHandle,  // IN: handle of an RSA key
    TPMT_RSA_DECRYPT* scheme      // IN: a sign or decrypt scheme
)
{
    TPMT_RSA_DECRYPT    place_holder;
    return scheme;
}

//*** CryptRsaLoadPrivateExponent()
// This function is called to generate the private exponent of an RSA key.
//  Return Type: TPM_RC
//      TPM_RC_BINDING      public and private parts of 'rsaKey' are not matched
TPM_RC
CryptRsaLoadPrivateExponent(TPMT_PUBLIC* publicArea, TPMT_SENSITIVE* sensitive)
{
    TPM_RC               retVal = TPM_RC_SUCCESS;
    return retVal;
}

static void ModExp(const uint8_t *base, size_t baseLen,
                   const uint8_t *exp, size_t expLen,
                   const uint8_t *mod, size_t modLen,
                   uint8_t *result, size_t resultLen)
{
    // For PoC, we assume all numbers are small enough to fit in a big-endian uint32_t.
    // Real implementation must use bignum library or TPM2's internal math routines.
    LOG_INFO("==== ModExp() Called ====\n");
    static uint32_t kTestPublicExponent = 65537;
    //Create Public Key
    uint32_t kTestModulus[kRsa2048NumWords];
    memcpy(kTestModulus, mod, (uint32_t)modLen);
    for (int i = 0; i < kRsa2048NumWords; i += 54) {
    char debug_str[1024]; // Bigger buffer to handle 54 bytes
    int len = 0;

    len += snprintf(debug_str + len, sizeof(debug_str) - len, "TPM MOD[%d]:", i);

    for (int j = 0; j < 54 && (i + j) < kRsa2048NumWords; j++) {
        len += snprintf(debug_str + len, sizeof(debug_str) - len, " %02x", kTestModulus[i + j]);
    }

    LOG_INFO("%s", debug_str);
}

    uint32_t public_key_data[ceil_div(kOtcryptoRsa2048PublicKeyBytes,
                                sizeof(uint32_t))];
      otcrypto_const_word32_buf_t modulus = {
      .data = kTestModulus,
      .len = ARRAYSIZE(kTestModulus),
  };
    otcrypto_unblinded_key_t public_key = {
        .key_mode = kOtcryptoKeyModeRsaEncryptOaep,
        .key_length = kOtcryptoRsa2048PublicKeyBytes,
        .key = public_key_data,
    };
    otcrypto_rsa_public_key_construct(kOtcryptoRsaSize2048, modulus,
                                        kTestPublicExponent, &public_key);
    //Generate CipherText stuff
    uint32_t ciphertext [64];
      otcrypto_word32_buf_t ciphertext_buf = {
      .data = ciphertext,
      .len = kRsa2048NumWords,
    };
    //Generate Message and label stuff
    otcrypto_const_byte_buf_t msg_buf = {.data = base, .len = baseLen};
    otcrypto_const_byte_buf_t label_buf = {.data = kTestLabel, .len = kTestLabelLen};
    LOG_INFO("Encrypting on the OTBN");
    otcrypto_rsa_encrypt(&public_key, kTestHashMode, msg_buf, label_buf,
                           ciphertext_buf);
    LOG_INFO("Done Encrypting on the OTBN");

    // for(int i = 0; i < ciphertext_buf.len; i ++){
    //     LOG_INFO("CT Buf[%d] = %08x", i, (uint32_t)ciphertext_buf.data[i]);
    // }
    memcpy(result, ciphertext_buf.data, ciphertext_buf.len * sizeof(uint32_t) );
    // for (int i = 0; i < ciphertext_buf.len; i += 16) {
    // char debug_str[512]; // Buffer for the formatted string
    // int len = 0;
    
    // len += snprintf(debug_str + len, sizeof(debug_str) - len, "TPM RSA Result[%d]:", i);
    
    // for (int j = 0; j < 16 && (i + j) < ciphertext_buf.len; j++) {
    //     len += snprintf(debug_str + len, sizeof(debug_str) - len, " %02x", ciphertext_buf.data[i + j]);
    // }
    
    // LOG_INFO("%s", debug_str);
    // }

}

//*** CryptRsaEncrypt()
// This is the entry point for encryption using RSA. Encryption is
// use of the public exponent. The padding parameter determines what
// padding will be used.
//
// The 'cOutSize' parameter must be at least as large as the size of the key.
//
// If the padding is RSA_PAD_NONE, 'dIn' is treated as a number. It must be
// lower in value than the key modulus.
// NOTE: If dIn has fewer bytes than cOut, then we don't add low-order zeros to
//       dIn to make it the size of the RSA key for the call to RSAEP. This is
//       because the high order bytes of dIn might have a numeric value that is
//       greater than the value of the key modulus. If this had low-order zeros
//       added, it would have a numeric value larger than the modulus even though
//       it started out with a lower numeric value.
//
//  Return Type: TPM_RC
//      TPM_RC_VALUE     'cOutSize' is too small (must be the size
//                        of the modulus)
//      TPM_RC_SCHEME    'padType' is not a supported scheme
//
LIB_EXPORT TPM_RC CryptRsaEncrypt(
    TPM2B_PUBLIC_KEY_RSA* cOut,    // OUT: the encrypted data
    TPM2B*                dIn,     // IN: the data to encrypt
    OBJECT*               key,     // IN: the key used for encryption
    TPMT_RSA_DECRYPT*     scheme,  // IN: the type of padding and hash
                                   //     if needed
    const TPM2B* label,            // IN: in case it is needed
    RAND_STATE*  rand              // IN: random number generator
                                   //     state (mostly for testing)
)
{
    TPM_RC               retVal = TPM_RC_SUCCESS;
    TPM2B_PUBLIC_KEY_RSA dataIn;
    TPMT_PUBLIC* publicArea = &key->publicArea;
    TPM2B_PUBLIC_KEY_RSA* n = &publicArea->unique.rsa;
    TPMI_RSA_KEY_BITS keyBits = publicArea->parameters.rsaDetail.keyBits;
    UINT32 pubExpField = publicArea->parameters.rsaDetail.exponent;

    uint8_t* modulus = n->b.buffer;
    size_t modLen = n->b.size;

    uint8_t exponentBuf[4];
    size_t expLen = 0;
    uint32_t exponent = (pubExpField == 0) ? 65537 : pubExpField;
    for (int i = 3; i >= 0; --i) {
        exponentBuf[i] = exponent & 0xFF;
        exponent >>= 8;
        expLen++;
    }
    // if the input and output buffers are the same, copy the input to a scratch
    // buffer so that things don't get messed up.
    if(dIn == &cOut->b)
    {
        MemoryCopy2B(&dataIn.b, dIn, sizeof(dataIn.t.buffer));
        dIn = &dataIn.b;
    }
    // All encryption schemes return the same size of data
    cOut->t.size = key->publicArea.unique.rsa.t.size;

    // for (int i = 0; i < modLen; i += 16) {
    // char debug_str[512]; // Buffer for the formatted string
    // int len = 0;
    
    // len += snprintf(debug_str + len, sizeof(debug_str) - len, "TPM Public Key[%d]:", i);
    
    // for (int j = 0; j < 16 && (i + j) < modLen; j++) {
    //     len += snprintf(debug_str + len, sizeof(debug_str) - len, " %02x", modulus[i + j]);
    // }
    
    // LOG_INFO("%s", debug_str);
    // }

    // Clear output
    memset(cOut->b.buffer, 0, sizeof(cOut->b.buffer));
    cOut->b.size = modLen;

    ModExp(dIn->buffer, dIn->size,
           &exponentBuf[4 - expLen], expLen,
           modulus, modLen,
           cOut->b.buffer, modLen);
    return TPM_RC_SUCCESS;


    //COMMENTING OUT THE ORIGINAL IMPLEMENTATION AS IT DOESNT FIT IN RAM 
    // //

//     TPM_DO_SELF_TEST(scheme->scheme);

//     switch(scheme->scheme)
//     {
//         case TPM_ALG_NULL:  // 'raw' encryption
//         {
//             INT32 i;
//             INT32 dSize = dIn->size;
//             // dIn can have more bytes than cOut as long as the extra bytes
//             // are zero. Note: the more significant bytes of a number in a byte
//             // buffer are the bytes at the start of the array.
//             for(i = 0; (i < dSize) && (dIn->buffer[i] == 0); i++)
//                 ;
//             dSize -= i;
//             if(dSize > cOut->t.size)
//                 ERROR_EXIT(TPM_RC_VALUE);
//             // Pad cOut with zeros if dIn is smaller
//             memset(cOut->t.buffer, 0, cOut->t.size - dSize);
//             // And copy the rest of the value
//             memcpy(&cOut->t.buffer[cOut->t.size - dSize], &dIn->buffer[i], dSize);

//             // If the size of dIn is the same as cOut dIn could be larger than
//             // the modulus. If it is, then RSAEP() will catch it.
//         }
//         break;
//         case TPM_ALG_RSAES:
//             //retVal = RSAES_PKCS1v1_5Encode(&cOut->b, dIn, rand);
//             LOG_INFO("Unimplemented Function");
//             break;
//         case TPM_ALG_OAEP:
//             // retVal =
//             //     OaepEncode(&cOut->b, scheme->details.oaep.hashAlg, label, dIn, rand);
//             LOG_INFO("Unimplemented Funtion");
//             break;
//         default:
//             ERROR_EXIT(TPM_RC_SCHEME);
//             break;
//     }
//     // All the schemes that do padding will come here for the encryption step
//     // Check that the Encoding worked
//     if(retVal == TPM_RC_SUCCESS)
//         // Padding OK so do the encryption
//         retVal = RSAEP(&cOut->b, key);
// Exit:
    // return retVal;
}

//*** CryptRsaDecrypt()
// This is the entry point for decryption using RSA. Decryption is
// use of the private exponent. The 'padType' parameter determines what
// padding was used.
//
//  Return Type: TPM_RC
//      TPM_RC_SIZE        'cInSize' is not the same as the size of the public
//                          modulus of 'key'; or numeric value of the encrypted
//                          data is greater than the modulus
//      TPM_RC_VALUE       'dOutSize' is not large enough for the result
//      TPM_RC_SCHEME      'padType' is not supported
//
LIB_EXPORT TPM_RC CryptRsaDecrypt(
    TPM2B*            dOut,    // OUT: the decrypted data
    TPM2B*            cIn,     // IN: the data to decrypt
    OBJECT*           key,     // IN: the key to use for decryption
    TPMT_RSA_DECRYPT* scheme,  // IN: the padding scheme
    const TPM2B*      label    // IN: in case it is needed for the scheme
)
{
    LOG_INFO("IN TPM2_RSA_DECRYPT");
    TPM_RC               retVal = TPM_RC_SUCCESS;
    TPM2B_PUBLIC_KEY_RSA dataIn;
    TPMT_PUBLIC* publicArea = &key->publicArea;
    TPM2B_PUBLIC_KEY_RSA* n = &publicArea->unique.rsa;
    TPMI_RSA_KEY_BITS keyBits = publicArea->parameters.rsaDetail.keyBits;
    UINT32 pubExpField = publicArea->parameters.rsaDetail.exponent;
    uint8_t * Priv_key = key->sensitive.sensitive.rsa.b.buffer;
    size_t priv_length = key->sensitive.sensitive.rsa.b.size;
    static uint32_t kTestPublicExponent = 65537;

    if((key->sensitive.sensitive.rsa.t.size & RSA_prime_flag) == 0)
    {
        if(CryptRsaLoadPrivateExponent(&key->publicArea, &key->sensitive)
           != TPM_RC_SUCCESS)
            return TPM_RC_BINDING;
    }


    for (int i = 0; i < priv_length; i += 16) {
    char debug_str[512]; // Buffer for the formatted string
    int len = 0;
    
    len += snprintf(debug_str + len, sizeof(debug_str) - len, "TPM Private Key[%d]:", i);
    
    for (int j = 0; j < 16 && (i + j) < priv_length; j++) {
        len += snprintf(debug_str + len, sizeof(debug_str) - len, " %02x", Priv_key[i + j]);
    }
    
    LOG_INFO("%s", debug_str);
    }
    //Set up the private exponent
    uint32_t kTestPrivateExponent[kRsa2048NumWords];
    memcpy(kTestPrivateExponent, Priv_key, (uint32_t)priv_length);
    
    uint8_t* mod = n->b.buffer;
    size_t modLen = n->b.size;

    uint32_t kTestModulus[kRsa2048NumWords];
    memcpy(kTestModulus, mod, (uint32_t)modLen);

    otcrypto_const_word32_buf_t d_share0 = {
      .data = kTestPrivateExponent,
      .len = ARRAYSIZE(kTestPrivateExponent),
  };
    uint32_t share1[ARRAYSIZE(kTestPrivateExponent)] = {0};
    otcrypto_const_word32_buf_t d_share1 = {
        .data = share1,
        .len = ARRAYSIZE(share1),
    };

    // Construct the private key.
    otcrypto_key_config_t private_key_config = {
        .version = kOtcryptoLibVersion1,
        .key_mode = kOtcryptoKeyModeRsaEncryptOaep,
        .key_length = kOtcryptoRsa2048PrivateKeyBytes,
        .hw_backed = kHardenedBoolFalse,
        .security_level = kOtcryptoKeySecurityLevelLow,
    };
    size_t keyblob_words =
        ceil_div(kOtcryptoRsa2048PrivateKeyblobBytes, sizeof(uint32_t));
    uint32_t keyblob[keyblob_words];
    otcrypto_blinded_key_t private_key = {
        .config = private_key_config,
        .keyblob = keyblob,
        .keyblob_length = kOtcryptoRsa2048PrivateKeyblobBytes,
    };
  otcrypto_const_word32_buf_t modulus = {
      .data = kTestModulus,
      .len = ARRAYSIZE(kTestModulus),
  };
    otcrypto_rsa_private_key_from_exponents(kOtcryptoRsaSize2048, modulus,
                                              kTestPublicExponent, d_share0,
                                              d_share1, &private_key);

    uint8_t pt[5] = {0x11, 0x22, 0x33, 0x44, 0x55};
    size_t pt_size = 5;
    otcrypto_byte_buf_t plaintext_buf = {.data = pt, .len = pt_size};
    otcrypto_const_byte_buf_t label_buf = {.data = kTestLabel, .len = kTestLabelLen};

    uint32_t ciphertext[kRsa2048NumWords];
    memcpy(ciphertext, cIn->buffer, kRsa2048NumWords* sizeof(uint32_t));
    otcrypto_const_word32_buf_t ciphertext_buf = {
        .data = ciphertext,
        .len = kRsa2048NumWords,
    };

    for (int i = 0; i < ciphertext_buf.len; i += 16) {
    char debug_str[512]; // Buffer for the formatted string
    int len = 0;
    
    len += snprintf(debug_str + len, sizeof(debug_str) - len, "TPM CT[%d]:", i);
    
    for (int j = 0; j < 16 && (i + j) < ciphertext_buf.len; j++) {
        len += snprintf(debug_str + len, sizeof(debug_str) - len, " %02x", ciphertext_buf.data[i + j]);
    }
    
    LOG_INFO("%s", debug_str);
    }
    LOG_INFO("Starting Decrypt");
    otcrypto_rsa_decrypt(&private_key, kTestHashMode, ciphertext_buf,
                            label_buf, plaintext_buf, &pt_size);
    LOG_INFO("Ending Decrypting");
    memcpy(dOut->buffer, plaintext_buf.data, plaintext_buf.len);





    // // Make sure that the necessary parameters are provided
    // pAssert(cIn != NULL && dOut != NULL && key != NULL);

    // // Size is checked to make sure that the encrypted value is the right size
    // if(cIn->size != key->publicArea.unique.rsa.t.size)
    //     ERROR_EXIT(TPM_RC_SIZE);

    // TPM_DO_SELF_TEST(scheme->scheme);

    return retVal;
}

//*** CryptRsaSign()
// This function is used to generate an RSA signature of the type indicated in
// 'scheme'.
//
//  Return Type: TPM_RC
//      TPM_RC_SCHEME       'scheme' or 'hashAlg' are not supported
//      TPM_RC_VALUE        'hInSize' does not match 'hashAlg' (for RSASSA)
//
LIB_EXPORT TPM_RC CryptRsaSign(TPMT_SIGNATURE* sigOut,
                               OBJECT*         key,  // IN: key to use
                               TPM2B_DIGEST*   hIn,  // IN: the digest to sign
                               RAND_STATE* rand  // IN: the random number generator
                                                 //      to use (mostly for testing)
)
{
    TPM_RC               retVal = TPM_RC_SUCCESS;
    return retVal;
}

//*** CryptRsaValidateSignature()
// This function is used to validate an RSA signature. If the signature is valid
// TPM_RC_SUCCESS is returned. If the signature is not valid, TPM_RC_SIGNATURE is
// returned. Other return codes indicate either parameter problems or fatal errors.
//
//  Return Type: TPM_RC
//      TPM_RC_SIGNATURE    the signature does not check
//      TPM_RC_SCHEME       unsupported scheme or hash algorithm
//
LIB_EXPORT TPM_RC CryptRsaValidateSignature(
    TPMT_SIGNATURE* sig,    // IN: signature
    OBJECT*         key,    // IN: public modulus
    TPM2B_DIGEST*   digest  // IN: The digest being validated
)
{
    TPM_RC               retVal = TPM_RC_SUCCESS;
    return retVal;

}

//*** CryptRsaGenerateKey()
// Generate an RSA key from a provided seed
/*(See part 1 specification)
//  The formulation is:
//      KDFa(hash, seed, label, Name, Counter, bits)
//  Where:
//      hash        the nameAlg from the public template
//      seed        a seed (will be a primary seed for a primary key)
//      label       a distinguishing label including vendor ID and
//                  vendor-assigned part number for the TPM.
//      Name        the nameAlg from the template and the hash of the template
//                  using nameAlg.
//      Counter     a 32-bit integer that is incremented each time the KDF is
//                  called in order to produce a specific key. This value
//                  can be a 32-bit integer in host format and does not need
//                  to be put in canonical form.
//      bits        the number of bits needed for the key.
//  The following process is implemented to find a RSA key pair:
//  1. pick a random number with enough bits from KDFa as a prime candidate
//  2. set the first two significant bits and the least significant bit of the
//     prime candidate
//  3. check if the number is a prime. if not, pick another random number
//  4. Make sure the difference between the two primes are more than 2^104.
//     Otherwise, restart the process for the second prime
//  5. If the counter has reached its maximum but we still can not find a valid
//     RSA key pair, return an internal error. This is an artificial bound.
//     Other implementation may choose a smaller number to indicate how many
//     times they are willing to try.
*/
//  Return Type: TPM_RC
//      TPM_RC_CANCELED     operation was canceled
//      TPM_RC_RANGE        public exponent is not supported
//      TPM_RC_VALUE        could not find a prime using the provided parameters
//#define FPGA
LIB_EXPORT TPM_RC CryptRsaGenerateKey(
    TPMT_PUBLIC*    publicArea,
    TPMT_SENSITIVE* sensitive,
    RAND_STATE*     rand  // IN: if not NULL, the deterministic
                          //     RNG state
)
{
    LOG_INFO("Creating RSA Key...");
    TPM_RC               retVal = TPM_RC_SUCCESS;

    UINT32 e = publicArea->parameters.rsaDetail.exponent;
    

    e = publicArea->parameters.rsaDetail.exponent;
    if(e == 0)
        e = RSA_DEFAULT_PUBLIC_EXPONENT;
    else
    {
        if(e < 65537)
            return (TPM_RC_RANGE);
        // Check that e is prime
        if(!IsPrimeInt(e))
            return (TPM_RC_RANGE);
    }
    int keySizeInBits = publicArea->parameters.rsaDetail.keyBits;
    if(((keySizeInBits % 1024) != 0)
       || (keySizeInBits > MAX_RSA_KEY_BITS)  // this might be redundant, but...
       || (keySizeInBits == 0))
        return (TPM_RC_VALUE);
    int keysizeBytes = (keySizeInBits / 8) + 4;
    static const otcrypto_key_mode_t kTestKeyMode = kOtcryptoKeyModeRsaEncryptOaep;
      // Allocate buffer for the public key.
  uint32_t public_key_data[ceil_div(keysizeBytes,
                                    sizeof(uint32_t))];
  memset(public_key_data, 0, sizeof(public_key_data));
  otcrypto_unblinded_key_t public_key = {
      .key_mode = kTestKeyMode,
      .key_length = keysizeBytes,
      .key = public_key_data,
  };

  // Allocate buffers for the private key.
  size_t keyblob_words =
      ceil_div(kOtcryptoRsa2048PrivateKeyblobBytes, sizeof(uint32_t));
  uint32_t keyblob[keyblob_words];
  memset(keyblob, 0, sizeof(keyblob));
  otcrypto_blinded_key_t private_key = {
      .config =
          {
              .version = kOtcryptoLibVersion1,
              .key_mode = kTestKeyMode,
              .key_length = kOtcryptoRsa2048PrivateKeyBytes,
              .hw_backed = kHardenedBoolFalse,
              .security_level = kOtcryptoKeySecurityLevelLow,
          },
      .keyblob_length = kOtcryptoRsa2048PrivateKeyblobBytes,
      .keyblob = keyblob,
  };

  // Generate the key pair.
  LOG_INFO("Starting keypair generation...");
  #ifdef FPGA
  otcrypto_status_t result =  otcrypto_rsa_keygen(kOtcryptoRsaSize2048, &public_key, &private_key);
  if( result.value != kHardenedBoolTrue){
    LOG_INFO("ERROR IN GENERATING KEY!!!!");
    return TPM_RC_CANCELED;
  }
  LOG_INFO("Keypair generation complete.");
  LOG_INFO("OTBN instruction count: %u", otbn_instruction_count_get());
  #endif
  #ifndef FPGA
uint32_t private_key_sim[] =  {
    0x6c66c0bf, 0x0a9720df, 0xbf16813d, 0x1006d4db, 0xe76f9093, 0xc68d9b59, 0x6c703b3c, 0x803f63ca,
    0xb7b0e6db, 0x9247a4cc, 0xec704511, 0x10e84df3, 0xf8794112, 0x42b68c69, 0x7c195b3f, 0xb4c1dcd7,
    0xa4df2bef, 0x401a2143, 0x8b49bfa7, 0xfece604f, 0xec2a322b, 0x9de1df67, 0xc4ea606f, 0xe8548a30,
    0x931919eb, 0x259000c0, 0x492504ac, 0x83886538, 0x8cd1c1d3, 0xeabcca44, 0x2e6ab03f, 0x72761454,
    0xdae77282, 0x641a386a, 0x5aec87e9, 0x7c2c27a8, 0xb310bdb1, 0x24180dab, 0xe50a06b0, 0xf0b9d4be,
    0xa2edc1ae, 0x404919e0, 0xab5928b4, 0x2389073f, 0x94b6bffc, 0xba01afec, 0x3c439fef, 0x1eb8f00f,
    0xa884296f, 0x33538bf2, 0x351cf456, 0xef9c542e, 0x122d0621, 0xfb80ac91, 0x06282451, 0x3e6f0a31,
    0x261d0068, 0x29e5b584, 0x50b28db3, 0xee663de4, 0x01590c53, 0xba3e5336, 0xfddb5a7c, 0x007c66af
};


uint32_t public_key_sim[] = {
    0xad0efdd7, 0x4df6eba5, 0x426b5cc5, 0xb944977a, 0x9e0fccef, 0x531bdd2d, 0xc66addd7, 0x6f0a6e37,
    0x972d932d, 0x91d66148, 0x673cdbf9, 0x77596bfe, 0x2897906f, 0x6e9bb09b, 0x22aee5fc, 0x7368ca56,
    0x1aaedb8a, 0xfe59186a, 0xab3e62e3, 0xe1b8c0e6, 0xd1e50cbc, 0x8960ae78, 0x87867298, 0x1d813a30,
    0x13637ce4, 0x008a3776, 0xb60e2b45, 0x277150e9, 0x38827c6a, 0x367f3f4b, 0x19d5d421, 0xf83cd22d,
    0x541e205c, 0xdf26b0e9, 0xfac0c2b5, 0x97365ed7, 0x7eec4216, 0xf763c308, 0x1b45e40e, 0xc7fc0d0a,
    0x19ed3afe, 0xc6b818f1, 0x5657a670, 0x7ea3ae34, 0xdf10c64f, 0x8d6be967, 0x8b370cb3, 0x4dc826e8,
    0x47adcf1b, 0x591ae1d1, 0x7d8007a9, 0x80e6ef42, 0x225225fb, 0x4a8d5129, 0xadc739d7, 0x5bf3e6cd,
    0xb3f54ca7, 0xa0f4941f, 0x92de3e53, 0x0e044bf0, 0xd7fdea3d, 0x75f06735, 0x8031d389, 0x7992fe51
};

rsa_2048_public_key_t tmp_key;
rsa_2048_int_t tmp_n;
memcpy(tmp_n.data, public_key_sim, 256);
tmp_key.n = tmp_n;
tmp_key.e = 0x10001;

memcpy(public_key.key, public_key_sim, 256);

private_key.keyblob = private_key_sim;

#endif

  // Interpret public key using internal RSA datatype.
  if(public_key.key_length != sizeof(rsa_2048_public_key_t)){
    LOG_INFO("Public Key Length not Correct");
    return TPM_RC_RANGE;
  }
  rsa_2048_public_key_t *pk = (rsa_2048_public_key_t *)public_key.key;

  // Interpret private key using internal RSA datatype.
  if(private_key.keyblob_length != sizeof(rsa_2048_private_key_t)){
    LOG_INFO("Private Key Length not Correct");
    return TPM_RC_RANGE;
  }
  rsa_2048_private_key_t *sk = (rsa_2048_private_key_t *)private_key.keyblob;

  // Check that the key uses the F4 exponent.
//   if(pk->e != 65537){
//     LOG_INFO("Exponenent Doesn't use F4 Exponent");
//     return TPM_RC_RANGE;
//   }

  // Check that the moduli match.
  if(ARRAYSIZE(pk->n.data) != ARRAYSIZE(sk->n.data)){
    LOG_INFO("Moduli don't match");
    return TPM_RC_RANGE;
  }

   //Copy the private exponent into the sensitive area
   sensitive->sensitive.rsa.t.size = keysizeBytes;
       for (size_t i = 0; i < (keysizeBytes) -1; i++) {
#ifdef LITTLE_ENDIAN_PACKING
        sensitive->sensitive.rsa.t.buffer[i * 4]     = (uint8_t)(private_key.keyblob[i] & 0xFF);
        sensitive->sensitive.rsa.t.buffer[i * 4 + 1] = (uint8_t)((private_key.keyblob[i] >> 8) & 0xFF);
        sensitive->sensitive.rsa.t.buffer[i * 4 + 2] = (uint8_t)((private_key.keyblob[i] >> 16) & 0xFF);
        sensitive->sensitive.rsa.t.buffer[i * 4 + 3] = (uint8_t)((private_key.keyblob[i] >> 24) & 0xFF);
#else
        sensitive->sensitive.rsa.t.buffer[i * 4]     = (uint8_t)((private_key.keyblob[i] >> 24) & 0xFF);
        sensitive->sensitive.rsa.t.buffer[i * 4 + 1] = (uint8_t)((private_key.keyblob[i] >> 16) & 0xFF);
        sensitive->sensitive.rsa.t.buffer[i * 4 + 2] = (uint8_t)((private_key.keyblob[i] >> 8) & 0xFF);
        sensitive->sensitive.rsa.t.buffer[i * 4 + 3] = (uint8_t)(private_key.keyblob[i] & 0xFF);
#endif
    }

          memcpy(publicArea->unique.rsa.t.buffer, public_key.key, 256);
//        for (size_t i = 0; i < (keysizeBytes) -1; i++) {
// #ifdef LITTLE_ENDIAN_PACKING
//        publicArea->unique.rsa.t.buffer[i * 4]     = (uint8_t)(public_key.key[i] & 0xFF);
//         publicArea->unique.rsa.t.buffer[i * 4 + 1] = (uint8_t)((public_key.key[i] >> 8) & 0xFF);
//         publicArea->unique.rsa.t.buffer[i * 4 + 2] = (uint8_t)((public_key.key[i] >> 16) & 0xFF);
//         publicArea->unique.rsa.t.buffer[i * 4 + 3] = (uint8_t)((public_key.key[i] >> 24) & 0xFF);
// #else
//         publicArea->unique.rsa.t.buffer[i * 4]     = (uint8_t)((public_key.key[i] >> 24) & 0xFF);
//         publicArea->unique.rsa.t.buffer[i * 4 + 1] = (uint8_t)((public_key.key[i] >> 16) & 0xFF);
//         publicArea->unique.rsa.t.buffer[i * 4 + 2] = (uint8_t)((public_key.key[i] >> 8) & 0xFF);
//         publicArea->unique.rsa.t.buffer[i * 4 + 3] = (uint8_t)(public_key.key[i] & 0xFF);
// #endif
//    }

   publicArea->unique.rsa.t.size = keysizeBytes;
    return retVal;
}

#endif  // ALG_RSA