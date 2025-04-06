#ifndef TPM_IMPL_H
#define TPM_IMPL_H

#define MAX_CAP_BUFFER             1024

#include <stdint.h>

#ifndef MAX
#  define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
#  define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

//BaseTypes Defines
typedef uint8_t  UINT8;
typedef uint8_t  BYTE;
typedef int8_t   INT8;
typedef int      BOOL;
typedef uint16_t UINT16;
typedef int16_t  INT16;
typedef uint32_t UINT32;
typedef int32_t  INT32;
typedef uint64_t UINT64;
typedef int64_t  INT64;


#include "sw/device/tests/TpmProfile_Common.h"
#include "sw/device/tests/TpmAlgorithmDefines.h"
#include "sw/device/tests/TpmProfile_ErrorCodes.h"
#include "sw/device/tests/TpmBuildSwitches.h"
#include "sw/device/tests/TpmTypes.h"



void ExecuteCommand(
    uint32_t        requestSize,   // IN: command buffer size
    unsigned char*  request,       // IN: command buffer
    uint32_t*       responseSize,  // IN/OUT: response buffer size
    unsigned char** response       // IN/OUT: response buffer
);


typedef BYTE UPDATE_TYPE;
#  define UT_NONE    (UPDATE_TYPE)0
#  define UT_NV      (UPDATE_TYPE)1
#  define UT_ORDERLY (UPDATE_TYPE)(UT_NV + 2)

extern UPDATE_TYPE g_updateNV;
extern BOOL g_clearOrderly;
extern BOOL g_inFailureMode;
extern BOOL g_daUsed;



#define FALSE 0
#define TRUE 1


#define YES   1
#define SET   1
#define NO    0
#define CLEAR 0


#define  ALG_YES                    YES
#define  ALG_NO                     NO

#define  MAX_COMMAND_SIZE               (4096-0x80)
#define  MAX_RESPONSE_SIZE              (4096-0x80)

#define STD_RESPONSE_HEADER (sizeof(TPM_ST) + sizeof(UINT32) + sizeof(TPM_RC))

//NV Macros

#  define SU_NONE_VALUE (0xFFFF)
#  define TPM_SU_NONE   (TPM_SU)(SU_NONE_VALUE)
#  define SU_DA_USED_VALUE (SU_NONE_VALUE - 1)
#  define TPM_SU_DA_USED   (TPM_SU)(SU_DA_USED_VALUE)

#define NV_IS_AVAILABLE (g_NvStatus == TPM_RC_SUCCESS)

#define IS_ORDERLY(value) (value < SU_DA_USED_VALUE)



#  define SU_NONE_VALUE (0xFFFF)
#  define TPM_SU_NONE   (TPM_SU)(SU_NONE_VALUE)

typedef UINT32 TPM_HANDLE;
#define TYPE_OF_TPM_HANDLE UINT32

typedef TPM_HANDLE TPMI_DH_OBJECT; 

extern TPMI_DH_OBJECT g_DRTMHandle;

typedef TPM_HANDLE TPM_RH;
#define TYPE_OF_TPM_RH              TPM_HANDLE
#define TPM_RH_FIRST                (TPM_RH)(0x40000000)
#define TPM_RH_SRK                  (TPM_RH)(0x40000000)
#define TPM_RH_OWNER                (TPM_RH)(0x40000001)
#define TPM_RH_REVOKE               (TPM_RH)(0x40000002)
#define TPM_RH_TRANSPORT            (TPM_RH)(0x40000003)
#define TPM_RH_OPERATOR             (TPM_RH)(0x40000004)
#define TPM_RH_ADMIN                (TPM_RH)(0x40000005)
#define TPM_RH_EK                   (TPM_RH)(0x40000006)
#define TPM_RH_NULL                 (TPM_RH)(0x40000007)
#define TPM_RH_UNASSIGNED           (TPM_RH)(0x40000008)
#define TPM_RS_PW                   (TPM_RH)(0x40000009)
#define TPM_RH_LOCKOUT              (TPM_RH)(0x4000000A)
#define TPM_RH_ENDORSEMENT          (TPM_RH)(0x4000000B)
#define TPM_RH_PLATFORM             (TPM_RH)(0x4000000C)
#define TPM_RH_PLATFORM_NV          (TPM_RH)(0x4000000D)
#define TPM_RH_AUTH_00              (TPM_RH)(0x40000010)
#define TPM_RH_AUTH_FF              (TPM_RH)(0x4000010F)
#define TPM_RH_ACT_0                (TPM_RH)(0x40000110)
#define TPM_RH_ACT_F                (TPM_RH)(0x4000011F)
#define TPM_RH_FW_OWNER             (TPM_RH)(0x40000140)
#define TPM_RH_FW_ENDORSEMENT       (TPM_RH)(0x40000141)
#define TPM_RH_FW_PLATFORM          (TPM_RH)(0x40000142)
#define TPM_RH_FW_NULL              (TPM_RH)(0x40000143)
#define TPM_RH_SVN_OWNER_BASE       (TPM_RH)(0x40010000)
#define TPM_RH_SVN_ENDORSEMENT_BASE (TPM_RH)(0x40020000)
#define TPM_RH_SVN_PLATFORM_BASE    (TPM_RH)(0x40030000)
#define TPM_RH_SVN_NULL_BASE        (TPM_RH)(0x40040000)
#define TPM_RH_LAST                 (TPM_RH)(0x4004FFFF)

//TPM_ST Defines
typedef UINT16 TPM_ST;
#define TYPE_OF_TPM_ST              UINT16
#define TPM_ST_RSP_COMMAND          (TPM_ST)(0x00C4)
#define TPM_ST_NULL                 (TPM_ST)(0x8000)
#define TPM_ST_NO_SESSIONS          (TPM_ST)(0x8001)
#define TPM_ST_SESSIONS             (TPM_ST)(0x8002)
#define TPM_ST_ATTEST_NV            (TPM_ST)(0x8014)
#define TPM_ST_ATTEST_COMMAND_AUDIT (TPM_ST)(0x8015)
#define TPM_ST_ATTEST_SESSION_AUDIT (TPM_ST)(0x8016)
#define TPM_ST_ATTEST_CERTIFY       (TPM_ST)(0x8017)
#define TPM_ST_ATTEST_QUOTE         (TPM_ST)(0x8018)
#define TPM_ST_ATTEST_TIME          (TPM_ST)(0x8019)
#define TPM_ST_ATTEST_CREATION      (TPM_ST)(0x801A)
#define TPM_ST_ATTEST_NV_DIGEST     (TPM_ST)(0x801C)
#define TPM_ST_CREATION             (TPM_ST)(0x8021)
#define TPM_ST_VERIFIED             (TPM_ST)(0x8022)
#define TPM_ST_AUTH_SECRET          (TPM_ST)(0x8023)
#define TPM_ST_HASHCHECK            (TPM_ST)(0x8024)
#define TPM_ST_AUTH_SIGNED          (TPM_ST)(0x8025)
#define TPM_ST_FU_MANIFEST          (TPM_ST)(0x8029)

//TPM Return Codes
typedef UINT32 TPM_RC;
#define TYPE_OF_TPM_RC           UINT32
#define TPM_RC_SUCCESS           (TPM_RC)(0x000)
#define TPM_RC_BAD_TAG           (TPM_RC)(0x01E)
#define RC_VER1                  (TPM_RC)(0x100)
#define TPM_RC_INITIALIZE        (TPM_RC)(RC_VER1 + 0x000)
#define TPM_RC_FAILURE           (TPM_RC)(RC_VER1 + 0x001)
#define TPM_RC_SEQUENCE          (TPM_RC)(RC_VER1 + 0x003)
#define TPM_RC_PRIVATE           (TPM_RC)(RC_VER1 + 0x00B)
#define TPM_RC_HMAC              (TPM_RC)(RC_VER1 + 0x019)
#define TPM_RC_DISABLED          (TPM_RC)(RC_VER1 + 0x020)
#define TPM_RC_EXCLUSIVE         (TPM_RC)(RC_VER1 + 0x021)
#define TPM_RC_AUTH_TYPE         (TPM_RC)(RC_VER1 + 0x024)
#define TPM_RC_AUTH_MISSING      (TPM_RC)(RC_VER1 + 0x025)
#define TPM_RC_POLICY            (TPM_RC)(RC_VER1 + 0x026)
#define TPM_RC_PCR               (TPM_RC)(RC_VER1 + 0x027)
#define TPM_RC_PCR_CHANGED       (TPM_RC)(RC_VER1 + 0x028)
#define TPM_RC_UPGRADE           (TPM_RC)(RC_VER1 + 0x02D)
#define TPM_RC_TOO_MANY_CONTEXTS (TPM_RC)(RC_VER1 + 0x02E)
#define TPM_RC_AUTH_UNAVAILABLE  (TPM_RC)(RC_VER1 + 0x02F)
#define TPM_RC_REBOOT            (TPM_RC)(RC_VER1 + 0x030)
#define TPM_RC_UNBALANCED        (TPM_RC)(RC_VER1 + 0x031)
#define TPM_RC_COMMAND_SIZE      (TPM_RC)(RC_VER1 + 0x042)
#define TPM_RC_COMMAND_CODE      (TPM_RC)(RC_VER1 + 0x043)
#define TPM_RC_AUTHSIZE          (TPM_RC)(RC_VER1 + 0x044)
#define TPM_RC_AUTH_CONTEXT      (TPM_RC)(RC_VER1 + 0x045)
#define TPM_RC_NV_RANGE          (TPM_RC)(RC_VER1 + 0x046)
#define TPM_RC_NV_SIZE           (TPM_RC)(RC_VER1 + 0x047)
#define TPM_RC_NV_LOCKED         (TPM_RC)(RC_VER1 + 0x048)
#define TPM_RC_NV_AUTHORIZATION  (TPM_RC)(RC_VER1 + 0x049)
#define TPM_RC_NV_UNINITIALIZED  (TPM_RC)(RC_VER1 + 0x04A)
#define TPM_RC_NV_SPACE          (TPM_RC)(RC_VER1 + 0x04B)
#define TPM_RC_NV_DEFINED        (TPM_RC)(RC_VER1 + 0x04C)
#define TPM_RC_BAD_CONTEXT       (TPM_RC)(RC_VER1 + 0x050)
#define TPM_RC_CPHASH            (TPM_RC)(RC_VER1 + 0x051)
#define TPM_RC_PARENT            (TPM_RC)(RC_VER1 + 0x052)
#define TPM_RC_NEEDS_TEST        (TPM_RC)(RC_VER1 + 0x053)
#define TPM_RC_NO_RESULT         (TPM_RC)(RC_VER1 + 0x054)
#define TPM_RC_SENSITIVE         (TPM_RC)(RC_VER1 + 0x055)
#define RC_MAX_FM0               (TPM_RC)(RC_VER1 + 0x07F)
#define RC_FMT1                  (TPM_RC)(0x080)
#define TPM_RC_ASYMMETRIC        (TPM_RC)(RC_FMT1 + 0x001)
#define TPM_RCS_ASYMMETRIC       (TPM_RC)(RC_FMT1 + 0x001)
#define TPM_RC_ATTRIBUTES        (TPM_RC)(RC_FMT1 + 0x002)
#define TPM_RCS_ATTRIBUTES       (TPM_RC)(RC_FMT1 + 0x002)
#define TPM_RC_HASH              (TPM_RC)(RC_FMT1 + 0x003)
#define TPM_RCS_HASH             (TPM_RC)(RC_FMT1 + 0x003)
#define TPM_RC_VALUE             (TPM_RC)(RC_FMT1 + 0x004)
#define TPM_RCS_VALUE            (TPM_RC)(RC_FMT1 + 0x004)
#define TPM_RC_HIERARCHY         (TPM_RC)(RC_FMT1 + 0x005)
#define TPM_RCS_HIERARCHY        (TPM_RC)(RC_FMT1 + 0x005)
#define TPM_RC_KEY_SIZE          (TPM_RC)(RC_FMT1 + 0x007)
#define TPM_RCS_KEY_SIZE         (TPM_RC)(RC_FMT1 + 0x007)
#define TPM_RC_MGF               (TPM_RC)(RC_FMT1 + 0x008)
#define TPM_RCS_MGF              (TPM_RC)(RC_FMT1 + 0x008)
#define TPM_RC_MODE              (TPM_RC)(RC_FMT1 + 0x009)
#define TPM_RCS_MODE             (TPM_RC)(RC_FMT1 + 0x009)
#define TPM_RC_TYPE              (TPM_RC)(RC_FMT1 + 0x00A)
#define TPM_RCS_TYPE             (TPM_RC)(RC_FMT1 + 0x00A)
#define TPM_RC_HANDLE            (TPM_RC)(RC_FMT1 + 0x00B)
#define TPM_RCS_HANDLE           (TPM_RC)(RC_FMT1 + 0x00B)
#define TPM_RC_KDF               (TPM_RC)(RC_FMT1 + 0x00C)
#define TPM_RCS_KDF              (TPM_RC)(RC_FMT1 + 0x00C)
#define TPM_RC_RANGE             (TPM_RC)(RC_FMT1 + 0x00D)
#define TPM_RCS_RANGE            (TPM_RC)(RC_FMT1 + 0x00D)
#define TPM_RC_AUTH_FAIL         (TPM_RC)(RC_FMT1 + 0x00E)
#define TPM_RCS_AUTH_FAIL        (TPM_RC)(RC_FMT1 + 0x00E)
#define TPM_RC_NONCE             (TPM_RC)(RC_FMT1 + 0x00F)
#define TPM_RCS_NONCE            (TPM_RC)(RC_FMT1 + 0x00F)
#define TPM_RC_PP                (TPM_RC)(RC_FMT1 + 0x010)
#define TPM_RCS_PP               (TPM_RC)(RC_FMT1 + 0x010)
#define TPM_RC_SCHEME            (TPM_RC)(RC_FMT1 + 0x012)
#define TPM_RCS_SCHEME           (TPM_RC)(RC_FMT1 + 0x012)
#define TPM_RC_SIZE              (TPM_RC)(RC_FMT1 + 0x015)
#define TPM_RCS_SIZE             (TPM_RC)(RC_FMT1 + 0x015)
#define TPM_RC_SYMMETRIC         (TPM_RC)(RC_FMT1 + 0x016)
#define TPM_RCS_SYMMETRIC        (TPM_RC)(RC_FMT1 + 0x016)
#define TPM_RC_TAG               (TPM_RC)(RC_FMT1 + 0x017)
#define TPM_RCS_TAG              (TPM_RC)(RC_FMT1 + 0x017)
#define TPM_RC_SELECTOR          (TPM_RC)(RC_FMT1 + 0x018)
#define TPM_RCS_SELECTOR         (TPM_RC)(RC_FMT1 + 0x018)
#define TPM_RC_INSUFFICIENT      (TPM_RC)(RC_FMT1 + 0x01A)
#define TPM_RCS_INSUFFICIENT     (TPM_RC)(RC_FMT1 + 0x01A)
#define TPM_RC_SIGNATURE         (TPM_RC)(RC_FMT1 + 0x01B)
#define TPM_RCS_SIGNATURE        (TPM_RC)(RC_FMT1 + 0x01B)
#define TPM_RC_KEY               (TPM_RC)(RC_FMT1 + 0x01C)
#define TPM_RCS_KEY              (TPM_RC)(RC_FMT1 + 0x01C)
#define TPM_RC_POLICY_FAIL       (TPM_RC)(RC_FMT1 + 0x01D)
#define TPM_RCS_POLICY_FAIL      (TPM_RC)(RC_FMT1 + 0x01D)
#define TPM_RC_INTEGRITY         (TPM_RC)(RC_FMT1 + 0x01F)
#define TPM_RCS_INTEGRITY        (TPM_RC)(RC_FMT1 + 0x01F)
#define TPM_RC_TICKET            (TPM_RC)(RC_FMT1 + 0x020)
#define TPM_RCS_TICKET           (TPM_RC)(RC_FMT1 + 0x020)
#define TPM_RC_RESERVED_BITS     (TPM_RC)(RC_FMT1 + 0x021)
#define TPM_RCS_RESERVED_BITS    (TPM_RC)(RC_FMT1 + 0x021)
#define TPM_RC_BAD_AUTH          (TPM_RC)(RC_FMT1 + 0x022)
#define TPM_RCS_BAD_AUTH         (TPM_RC)(RC_FMT1 + 0x022)
#define TPM_RC_EXPIRED           (TPM_RC)(RC_FMT1 + 0x023)
#define TPM_RCS_EXPIRED          (TPM_RC)(RC_FMT1 + 0x023)
#define TPM_RC_POLICY_CC         (TPM_RC)(RC_FMT1 + 0x024)
#define TPM_RCS_POLICY_CC        (TPM_RC)(RC_FMT1 + 0x024)
#define TPM_RC_BINDING           (TPM_RC)(RC_FMT1 + 0x025)
#define TPM_RCS_BINDING          (TPM_RC)(RC_FMT1 + 0x025)
#define TPM_RC_CURVE             (TPM_RC)(RC_FMT1 + 0x026)
#define TPM_RCS_CURVE            (TPM_RC)(RC_FMT1 + 0x026)
#define TPM_RC_ECC_POINT         (TPM_RC)(RC_FMT1 + 0x027)
#define TPM_RCS_ECC_POINT        (TPM_RC)(RC_FMT1 + 0x027)
#define TPM_RC_FW_LIMITED        (TPM_RC)(RC_FMT1 + 0x028)
#define TPM_RC_SVN_LIMITED       (TPM_RC)(RC_FMT1 + 0x029)
#define RC_WARN                  (TPM_RC)(0x900)
#define TPM_RC_CONTEXT_GAP       (TPM_RC)(RC_WARN + 0x001)
#define TPM_RC_OBJECT_MEMORY     (TPM_RC)(RC_WARN + 0x002)
#define TPM_RC_SESSION_MEMORY    (TPM_RC)(RC_WARN + 0x003)
#define TPM_RC_MEMORY            (TPM_RC)(RC_WARN + 0x004)
#define TPM_RC_SESSION_HANDLES   (TPM_RC)(RC_WARN + 0x005)
#define TPM_RC_OBJECT_HANDLES    (TPM_RC)(RC_WARN + 0x006)
#define TPM_RC_LOCALITY          (TPM_RC)(RC_WARN + 0x007)
#define TPM_RC_YIELDED           (TPM_RC)(RC_WARN + 0x008)
#define TPM_RC_CANCELED          (TPM_RC)(RC_WARN + 0x009)
#define TPM_RC_TESTING           (TPM_RC)(RC_WARN + 0x00A)
#define TPM_RC_REFERENCE_H0      (TPM_RC)(RC_WARN + 0x010)
#define TPM_RC_REFERENCE_H1      (TPM_RC)(RC_WARN + 0x011)
#define TPM_RC_REFERENCE_H2      (TPM_RC)(RC_WARN + 0x012)
#define TPM_RC_REFERENCE_H3      (TPM_RC)(RC_WARN + 0x013)
#define TPM_RC_REFERENCE_H4      (TPM_RC)(RC_WARN + 0x014)
#define TPM_RC_REFERENCE_H5      (TPM_RC)(RC_WARN + 0x015)
#define TPM_RC_REFERENCE_H6      (TPM_RC)(RC_WARN + 0x016)
#define TPM_RC_REFERENCE_S0      (TPM_RC)(RC_WARN + 0x018)
#define TPM_RC_REFERENCE_S1      (TPM_RC)(RC_WARN + 0x019)
#define TPM_RC_REFERENCE_S2      (TPM_RC)(RC_WARN + 0x01A)
#define TPM_RC_REFERENCE_S3      (TPM_RC)(RC_WARN + 0x01B)
#define TPM_RC_REFERENCE_S4      (TPM_RC)(RC_WARN + 0x01C)
#define TPM_RC_REFERENCE_S5      (TPM_RC)(RC_WARN + 0x01D)
#define TPM_RC_REFERENCE_S6      (TPM_RC)(RC_WARN + 0x01E)
#define TPM_RC_NV_RATE           (TPM_RC)(RC_WARN + 0x020)
#define TPM_RC_LOCKOUT           (TPM_RC)(RC_WARN + 0x021)
#define TPM_RC_RETRY             (TPM_RC)(RC_WARN + 0x022)
#define TPM_RC_NV_UNAVAILABLE    (TPM_RC)(RC_WARN + 0x023)
#define TPM_RC_NOT_USED          (TPM_RC)(RC_WARN + 0x7F)
#define TPM_RC_H                 (TPM_RC)(0x000)
#define TPM_RC_P                 (TPM_RC)(0x040)
#define TPM_RC_S                 (TPM_RC)(0x800)
#define TPM_RC_1                 (TPM_RC)(0x100)
#define TPM_RC_2                 (TPM_RC)(0x200)
#define TPM_RC_3                 (TPM_RC)(0x300)
#define TPM_RC_4                 (TPM_RC)(0x400)
#define TPM_RC_5                 (TPM_RC)(0x500)
#define TPM_RC_6                 (TPM_RC)(0x600)
#define TPM_RC_7                 (TPM_RC)(0x700)
#define TPM_RC_8                 (TPM_RC)(0x800)
#define TPM_RC_9                 (TPM_RC)(0x900)
#define TPM_RC_A                 (TPM_RC)(0xA00)
#define TPM_RC_B                 (TPM_RC)(0xB00)
#define TPM_RC_C                 (TPM_RC)(0xC00)
#define TPM_RC_D                 (TPM_RC)(0xD00)
#define TPM_RC_E                 (TPM_RC)(0xE00)
#define TPM_RC_F                 (TPM_RC)(0xF00)
#define TPM_RC_N_MASK            (TPM_RC)(0xF00)

typedef UINT32 TPM_CC;

#define TYPE_OF_TPM_CC                    UINT32
#define TPM_CC_FIRST                      (TPM_CC)(0x0000011F)
#define TPM_CC_NV_UndefineSpaceSpecial    (TPM_CC)(0x0000011F)
#define TPM_CC_EvictControl               (TPM_CC)(0x00000120)
#define TPM_CC_HierarchyControl           (TPM_CC)(0x00000121)
#define TPM_CC_NV_UndefineSpace           (TPM_CC)(0x00000122)
#define TPM_CC_ChangeEPS                  (TPM_CC)(0x00000124)
#define TPM_CC_ChangePPS                  (TPM_CC)(0x00000125)
#define TPM_CC_Clear                      (TPM_CC)(0x00000126)
#define TPM_CC_ClearControl               (TPM_CC)(0x00000127)
#define TPM_CC_ClockSet                   (TPM_CC)(0x00000128)
#define TPM_CC_HierarchyChangeAuth        (TPM_CC)(0x00000129)
#define TPM_CC_NV_DefineSpace             (TPM_CC)(0x0000012A)
#define TPM_CC_PCR_Allocate               (TPM_CC)(0x0000012B)
#define TPM_CC_PCR_SetAuthPolicy          (TPM_CC)(0x0000012C)
#define TPM_CC_PP_Commands                (TPM_CC)(0x0000012D)
#define TPM_CC_SetPrimaryPolicy           (TPM_CC)(0x0000012E)
#define TPM_CC_FieldUpgradeStart          (TPM_CC)(0x0000012F)
#define TPM_CC_ClockRateAdjust            (TPM_CC)(0x00000130)
#define TPM_CC_CreatePrimary              (TPM_CC)(0x00000131)
#define TPM_CC_NV_GlobalWriteLock         (TPM_CC)(0x00000132)
#define TPM_CC_GetCommandAuditDigest      (TPM_CC)(0x00000133)
#define TPM_CC_NV_Increment               (TPM_CC)(0x00000134)
#define TPM_CC_NV_SetBits                 (TPM_CC)(0x00000135)
#define TPM_CC_NV_Extend                  (TPM_CC)(0x00000136)
#define TPM_CC_NV_Write                   (TPM_CC)(0x00000137)
#define TPM_CC_NV_WriteLock               (TPM_CC)(0x00000138)
#define TPM_CC_DictionaryAttackLockReset  (TPM_CC)(0x00000139)
#define TPM_CC_DictionaryAttackParameters (TPM_CC)(0x0000013A)
#define TPM_CC_NV_ChangeAuth              (TPM_CC)(0x0000013B)
#define TPM_CC_PCR_Event                  (TPM_CC)(0x0000013C)
#define TPM_CC_PCR_Reset                  (TPM_CC)(0x0000013D)
#define TPM_CC_SequenceComplete           (TPM_CC)(0x0000013E)
#define TPM_CC_SetAlgorithmSet            (TPM_CC)(0x0000013F)
#define TPM_CC_SetCommandCodeAuditStatus  (TPM_CC)(0x00000140)
#define TPM_CC_FieldUpgradeData           (TPM_CC)(0x00000141)
#define TPM_CC_IncrementalSelfTest        (TPM_CC)(0x00000142)
#define TPM_CC_SelfTest                   (TPM_CC)(0x00000143)
#define TPM_CC_Startup                    (TPM_CC)(0x00000144)
#define TPM_CC_Shutdown                   (TPM_CC)(0x00000145)
#define TPM_CC_StirRandom                 (TPM_CC)(0x00000146)
#define TPM_CC_ActivateCredential         (TPM_CC)(0x00000147)
#define TPM_CC_Certify                    (TPM_CC)(0x00000148)
#define TPM_CC_PolicyNV                   (TPM_CC)(0x00000149)
#define TPM_CC_CertifyCreation            (TPM_CC)(0x0000014A)
#define TPM_CC_Duplicate                  (TPM_CC)(0x0000014B)
#define TPM_CC_GetTime                    (TPM_CC)(0x0000014C)
#define TPM_CC_GetSessionAuditDigest      (TPM_CC)(0x0000014D)
#define TPM_CC_NV_Read                    (TPM_CC)(0x0000014E)
#define TPM_CC_NV_ReadLock                (TPM_CC)(0x0000014F)
#define TPM_CC_ObjectChangeAuth           (TPM_CC)(0x00000150)
#define TPM_CC_PolicySecret               (TPM_CC)(0x00000151)
#define TPM_CC_Rewrap                     (TPM_CC)(0x00000152)
#define TPM_CC_Create                     (TPM_CC)(0x00000153)
#define TPM_CC_ECDH_ZGen                  (TPM_CC)(0x00000154)
#define TPM_CC_HMAC                       (TPM_CC)(0x00000155)
#define TPM_CC_MAC                        (TPM_CC)(0x00000155)
#define TPM_CC_Import                     (TPM_CC)(0x00000156)
#define TPM_CC_Load                       (TPM_CC)(0x00000157)
#define TPM_CC_Quote                      (TPM_CC)(0x00000158)
#define TPM_CC_RSA_Decrypt                (TPM_CC)(0x00000159)
#define TPM_CC_HMAC_Start                 (TPM_CC)(0x0000015B)
#define TPM_CC_MAC_Start                  (TPM_CC)(0x0000015B)
#define TPM_CC_SequenceUpdate             (TPM_CC)(0x0000015C)
#define TPM_CC_Sign                       (TPM_CC)(0x0000015D)
#define TPM_CC_Unseal                     (TPM_CC)(0x0000015E)
#define TPM_CC_PolicySigned               (TPM_CC)(0x00000160)
#define TPM_CC_ContextLoad                (TPM_CC)(0x00000161)
#define TPM_CC_ContextSave                (TPM_CC)(0x00000162)
#define TPM_CC_ECDH_KeyGen                (TPM_CC)(0x00000163)
#define TPM_CC_EncryptDecrypt             (TPM_CC)(0x00000164)
#define TPM_CC_FlushContext               (TPM_CC)(0x00000165)
#define TPM_CC_LoadExternal               (TPM_CC)(0x00000167)
#define TPM_CC_MakeCredential             (TPM_CC)(0x00000168)
#define TPM_CC_NV_ReadPublic              (TPM_CC)(0x00000169)
#define TPM_CC_PolicyAuthorize            (TPM_CC)(0x0000016A)
#define TPM_CC_PolicyAuthValue            (TPM_CC)(0x0000016B)
#define TPM_CC_PolicyCommandCode          (TPM_CC)(0x0000016C)
#define TPM_CC_PolicyCounterTimer         (TPM_CC)(0x0000016D)
#define TPM_CC_PolicyCpHash               (TPM_CC)(0x0000016E)
#define TPM_CC_PolicyLocality             (TPM_CC)(0x0000016F)
#define TPM_CC_PolicyNameHash             (TPM_CC)(0x00000170)
#define TPM_CC_PolicyOR                   (TPM_CC)(0x00000171)
#define TPM_CC_PolicyTicket               (TPM_CC)(0x00000172)
#define TPM_CC_ReadPublic                 (TPM_CC)(0x00000173)
#define TPM_CC_RSA_Encrypt                (TPM_CC)(0x00000174)
#define TPM_CC_StartAuthSession           (TPM_CC)(0x00000176)
#define TPM_CC_VerifySignature            (TPM_CC)(0x00000177)
#define TPM_CC_ECC_Parameters             (TPM_CC)(0x00000178)
#define TPM_CC_FirmwareRead               (TPM_CC)(0x00000179)
#define TPM_CC_GetCapability              (TPM_CC)(0x0000017A)
#define TPM_CC_GetRandom                  (TPM_CC)(0x0000017B)
#define TPM_CC_GetTestResult              (TPM_CC)(0x0000017C)
#define TPM_CC_Hash                       (TPM_CC)(0x0000017D)
#define TPM_CC_PCR_Read                   (TPM_CC)(0x0000017E)
#define TPM_CC_PolicyPCR                  (TPM_CC)(0x0000017F)
#define TPM_CC_PolicyRestart              (TPM_CC)(0x00000180)
#define TPM_CC_ReadClock                  (TPM_CC)(0x00000181)
#define TPM_CC_PCR_Extend                 (TPM_CC)(0x00000182)
#define TPM_CC_PCR_SetAuthValue           (TPM_CC)(0x00000183)
#define TPM_CC_NV_Certify                 (TPM_CC)(0x00000184)
#define TPM_CC_EventSequenceComplete      (TPM_CC)(0x00000185)
#define TPM_CC_HashSequenceStart          (TPM_CC)(0x00000186)
#define TPM_CC_PolicyPhysicalPresence     (TPM_CC)(0x00000187)
#define TPM_CC_PolicyDuplicationSelect    (TPM_CC)(0x00000188)
#define TPM_CC_PolicyGetDigest            (TPM_CC)(0x00000189)
#define TPM_CC_TestParms                  (TPM_CC)(0x0000018A)
#define TPM_CC_Commit                     (TPM_CC)(0x0000018B)
#define TPM_CC_PolicyPassword             (TPM_CC)(0x0000018C)
#define TPM_CC_ZGen_2Phase                (TPM_CC)(0x0000018D)
#define TPM_CC_EC_Ephemeral               (TPM_CC)(0x0000018E)
#define TPM_CC_PolicyNvWritten            (TPM_CC)(0x0000018F)
#define TPM_CC_PolicyTemplate             (TPM_CC)(0x00000190)
#define TPM_CC_CreateLoaded               (TPM_CC)(0x00000191)
#define TPM_CC_PolicyAuthorizeNV          (TPM_CC)(0x00000192)
#define TPM_CC_EncryptDecrypt2            (TPM_CC)(0x00000193)
#define TPM_CC_AC_GetCapability           (TPM_CC)(0x00000194)
#define TPM_CC_AC_Send                    (TPM_CC)(0x00000195)
#define TPM_CC_Policy_AC_SendSelect       (TPM_CC)(0x00000196)
#define TPM_CC_CertifyX509                (TPM_CC)(0x00000197)
#define TPM_CC_ACT_SetTimeout             (TPM_CC)(0x00000198)
#define TPM_CC_ECC_Encrypt                (TPM_CC)(0x00000199)
#define TPM_CC_ECC_Decrypt                (TPM_CC)(0x0000019A)
#define TPM_CC_PolicyCapability           (TPM_CC)(0x0000019B)
#define TPM_CC_PolicyParameters           (TPM_CC)(0x0000019C)
#define TPM_CC_NV_DefineSpace2            (TPM_CC)(0x0000019D)
#define TPM_CC_NV_ReadPublic2             (TPM_CC)(0x0000019E)
#define TPM_CC_SetCapability              (TPM_CC)(0x0000019F)
#define TPM_CC_LAST                       (TPM_CC)(0x0000019F)
#define CC_VEND                           (TPM_CC)(0x20000000)
#define TPM_CC_Vendor_TCG_Test            (TPM_CC)(0x20000000)

typedef TPM_CC TPMA_CC;
#  define TPMA_CC_commandIndex       (TPMA_CC)(0xFFFF << 0)
#  define TPMA_CC_commandIndex_SHIFT 0
#  define TPMA_CC_nv                 (TPMA_CC)(1 << 22)
#  define TPMA_CC_extensive          (TPMA_CC)(1 << 23)
#  define TPMA_CC_flushed            (TPMA_CC)(1 << 24)
#  define TPMA_CC_cHandles           (TPMA_CC)(7 << 25)
#  define TPMA_CC_cHandles_SHIFT     25
#  define TPMA_CC_rHandle            (TPMA_CC)(1 << 28)
#  define TPMA_CC_V                  (TPMA_CC)(1 << 29)



typedef UINT16 COMMAND_INDEX;
#  define UNIMPLEMENTED_COMMAND_INDEX ((COMMAND_INDEX)(~0))

#  define CP_HASH(HASH, Hash) TPM2B_##HASH##_DIGEST Hash##CpHash;
#  define RP_HASH(HASH, Hash) TPM2B_##HASH##_DIGEST Hash##RpHash;

#define MAX_HANDLE_NUM             3

//Struct Command
typedef struct COMMAND
{
    TPM_ST        tag;                   // the parsed command tag
    TPM_CC        code;                  // the parsed command code
    COMMAND_INDEX index;                 // the computed command index
    UINT32        handleNum;             // the number of entity handles in the
                                         //   handle area of the command
    TPM_HANDLE handles[MAX_HANDLE_NUM];  // the parsed handle values
    UINT32     sessionNum;               // the number of sessions found
    INT32      parameterSize;            // starts out with the parsed command size
                                         // and is reduced and values are
                                         // unmarshaled. Just before calling the
                                         // command actions, this should be zero.
                                         // After the command actions, this number
                                         // should grow as values are marshaled
                                         // in to the response buffer.
    INT32 authSize;                      // this is initialized with the parsed size
                                         // of authorizationSize field and should
                                         // be zero when the authorizations are
                                         // parsed.
    BYTE* parameterBuffer;               // input to ExecuteCommand
    BYTE* responseBuffer;                // input to ExecuteCommand
} COMMAND;

typedef UINT16 TPM_ALG_ID;
#define TYPE_OF_TPM_ALG_ID       UINT16
#define ALG_ERROR_VALUE          0x0000
#define TPM_ALG_ERROR            (TPM_ALG_ID)(ALG_ERROR_VALUE)
#define ALG_RSA_VALUE            0x0001
#define TPM_ALG_RSA              (TPM_ALG_ID)(ALG_RSA_VALUE)
#define ALG_TDES_VALUE           0x0003
#define TPM_ALG_TDES             (TPM_ALG_ID)(ALG_TDES_VALUE)
#define ALG_SHA1_VALUE           0x0004
#define TPM_ALG_SHA1             (TPM_ALG_ID)(ALG_SHA1_VALUE)
#define ALG_HMAC_VALUE           0x0005
#define TPM_ALG_HMAC             (TPM_ALG_ID)(ALG_HMAC_VALUE)
#define ALG_AES_VALUE            0x0006
#define TPM_ALG_AES              (TPM_ALG_ID)(ALG_AES_VALUE)
#define ALG_MGF1_VALUE           0x0007
#define TPM_ALG_MGF1             (TPM_ALG_ID)(ALG_MGF1_VALUE)
#define ALG_KEYEDHASH_VALUE      0x0008
#define TPM_ALG_KEYEDHASH        (TPM_ALG_ID)(ALG_KEYEDHASH_VALUE)
#define ALG_XOR_VALUE            0x000A
#define TPM_ALG_XOR              (TPM_ALG_ID)(ALG_XOR_VALUE)
#define ALG_SHA256_VALUE         0x000B
#define TPM_ALG_SHA256           (TPM_ALG_ID)(ALG_SHA256_VALUE)
#define ALG_SHA384_VALUE         0x000C
#define TPM_ALG_SHA384           (TPM_ALG_ID)(ALG_SHA384_VALUE)
#define ALG_SHA512_VALUE         0x000D
#define TPM_ALG_SHA512           (TPM_ALG_ID)(ALG_SHA512_VALUE)
#define ALG_SHA256_192_VALUE     0x000E
#define TPM_ALG_SHA256_192       (TPM_ALG_ID)(ALG_SHA256_192_VALUE)
#define ALG_NULL_VALUE           0x0010
#define TPM_ALG_NULL             (TPM_ALG_ID)(ALG_NULL_VALUE)
#define ALG_SM3_256_VALUE        0x0012
#define TPM_ALG_SM3_256          (TPM_ALG_ID)(ALG_SM3_256_VALUE)
#define ALG_SM4_VALUE            0x0013
#define TPM_ALG_SM4              (TPM_ALG_ID)(ALG_SM4_VALUE)
#define ALG_RSASSA_VALUE         0x0014
#define TPM_ALG_RSASSA           (TPM_ALG_ID)(ALG_RSASSA_VALUE)
#define ALG_RSAES_VALUE          0x0015
#define TPM_ALG_RSAES            (TPM_ALG_ID)(ALG_RSAES_VALUE)
#define ALG_RSAPSS_VALUE         0x0016
#define TPM_ALG_RSAPSS           (TPM_ALG_ID)(ALG_RSAPSS_VALUE)
#define ALG_OAEP_VALUE           0x0017
#define TPM_ALG_OAEP             (TPM_ALG_ID)(ALG_OAEP_VALUE)
#define ALG_ECDSA_VALUE          0x0018
#define TPM_ALG_ECDSA            (TPM_ALG_ID)(ALG_ECDSA_VALUE)
#define ALG_ECDH_VALUE           0x0019
#define TPM_ALG_ECDH             (TPM_ALG_ID)(ALG_ECDH_VALUE)
#define ALG_ECDAA_VALUE          0x001A
#define TPM_ALG_ECDAA            (TPM_ALG_ID)(ALG_ECDAA_VALUE)
#define ALG_SM2_VALUE            0x001B
#define TPM_ALG_SM2              (TPM_ALG_ID)(ALG_SM2_VALUE)
#define ALG_ECSCHNORR_VALUE      0x001C
#define TPM_ALG_ECSCHNORR        (TPM_ALG_ID)(ALG_ECSCHNORR_VALUE)
#define ALG_ECMQV_VALUE          0x001D
#define TPM_ALG_ECMQV            (TPM_ALG_ID)(ALG_ECMQV_VALUE)
#define ALG_KDF1_SP800_56A_VALUE 0x0020
#define TPM_ALG_KDF1_SP800_56A   (TPM_ALG_ID)(ALG_KDF1_SP800_56A_VALUE)
#define ALG_KDF2_VALUE           0x0021
#define TPM_ALG_KDF2             (TPM_ALG_ID)(ALG_KDF2_VALUE)
#define ALG_KDF1_SP800_108_VALUE 0x0022
#define TPM_ALG_KDF1_SP800_108   (TPM_ALG_ID)(ALG_KDF1_SP800_108_VALUE)
#define ALG_ECC_VALUE            0x0023
#define TPM_ALG_ECC              (TPM_ALG_ID)(ALG_ECC_VALUE)
#define ALG_SYMCIPHER_VALUE      0x0025
#define TPM_ALG_SYMCIPHER        (TPM_ALG_ID)(ALG_SYMCIPHER_VALUE)
#define ALG_CAMELLIA_VALUE       0x0026
#define TPM_ALG_CAMELLIA         (TPM_ALG_ID)(ALG_CAMELLIA_VALUE)
#define ALG_SHA3_256_VALUE       0x0027
#define TPM_ALG_SHA3_256         (TPM_ALG_ID)(ALG_SHA3_256_VALUE)
#define ALG_SHA3_384_VALUE       0x0028
#define TPM_ALG_SHA3_384         (TPM_ALG_ID)(ALG_SHA3_384_VALUE)
#define ALG_SHA3_512_VALUE       0x0029
#define TPM_ALG_SHA3_512         (TPM_ALG_ID)(ALG_SHA3_512_VALUE)
#define ALG_SHAKE128_VALUE       0x002A
#define TPM_ALG_SHAKE128         (TPM_ALG_ID)(ALG_SHAKE128_VALUE)
#define ALG_SHAKE256_VALUE       0x002B
#define TPM_ALG_SHAKE256         (TPM_ALG_ID)(ALG_SHAKE256_VALUE)
#define ALG_SHAKE256_192_VALUE   0x002C
#define TPM_ALG_SHAKE256_192     (TPM_ALG_ID)(ALG_SHAKE256_192_VALUE)
#define ALG_SHAKE256_256_VALUE   0x002D
#define TPM_ALG_SHAKE256_256     (TPM_ALG_ID)(ALG_SHAKE256_256_VALUE)
#define ALG_SHAKE256_512_VALUE   0x002E
#define TPM_ALG_SHAKE256_512     (TPM_ALG_ID)(ALG_SHAKE256_512_VALUE)
#define ALG_CMAC_VALUE           0x003F
#define TPM_ALG_CMAC             (TPM_ALG_ID)(ALG_CMAC_VALUE)
#define ALG_CTR_VALUE            0x0040
#define TPM_ALG_CTR              (TPM_ALG_ID)(ALG_CTR_VALUE)
#define ALG_OFB_VALUE            0x0041
#define TPM_ALG_OFB              (TPM_ALG_ID)(ALG_OFB_VALUE)
#define ALG_CBC_VALUE            0x0042
#define TPM_ALG_CBC              (TPM_ALG_ID)(ALG_CBC_VALUE)
#define ALG_CFB_VALUE            0x0043
#define TPM_ALG_CFB              (TPM_ALG_ID)(ALG_CFB_VALUE)
#define ALG_ECB_VALUE            0x0044
#define TPM_ALG_ECB              (TPM_ALG_ID)(ALG_ECB_VALUE)
#define ALG_CCM_VALUE            0x0050
#define TPM_ALG_CCM              (TPM_ALG_ID)(ALG_CCM_VALUE)
#define ALG_GCM_VALUE            0x0051
#define TPM_ALG_GCM              (TPM_ALG_ID)(ALG_GCM_VALUE)
#define ALG_KW_VALUE             0x0052
#define TPM_ALG_KW               (TPM_ALG_ID)(ALG_KW_VALUE)
#define ALG_KWP_VALUE            0x0053
#define TPM_ALG_KWP              (TPM_ALG_ID)(ALG_KWP_VALUE)
#define ALG_EAX_VALUE            0x0054
#define TPM_ALG_EAX              (TPM_ALG_ID)(ALG_EAX_VALUE)
#define ALG_EDDSA_VALUE          0x0060
#define TPM_ALG_EDDSA            (TPM_ALG_ID)(ALG_EDDSA_VALUE)
#define ALG_EDDSA_PH_VALUE       0x0061
#define TPM_ALG_EDDSA_PH         (TPM_ALG_ID)(ALG_EDDSA_PH_VALUE)
#define ALG_LMS_VALUE            0x0070
#define TPM_ALG_LMS              (TPM_ALG_ID)(ALG_LMS_VALUE)
#define ALG_XMSS_VALUE           0x0071
#define TPM_ALG_XMSS             (TPM_ALG_ID)(ALG_XMSS_VALUE)
//        Values derived from Table "Definition of TPM_ALG_ID Constants" (Part 2: Structures)
#define ALG_FIRST_VALUE 0x0001
#define TPM_ALG_FIRST   (TPM_ALG_ID)(ALG_FIRST_VALUE)
#define ALG_LAST_VALUE  0x0071
#define TPM_ALG_LAST    (TPM_ALG_ID)(ALG_LAST_VALUE)


//Persistent Data Macros and Dependencies

#define PERSISTENT_DATA_PLATFORM_SPACE  16

typedef BYTE       TPMI_YES_NO;               // (Part 2: Structures)
typedef TPM_HANDLE TPMI_DH_OBJECT;            // (Part 2: Structures)
typedef TPM_HANDLE TPMI_DH_PARENT;            // (Part 2: Structures)
typedef TPM_HANDLE TPMI_DH_PERSISTENT;        // (Part 2: Structures)
typedef TPM_HANDLE TPMI_DH_ENTITY;            // (Part 2: Structures)
typedef TPM_HANDLE TPMI_DH_PCR;               // (Part 2: Structures)
typedef TPM_HANDLE TPMI_SH_AUTH_SESSION;      // (Part 2: Structures)
typedef TPM_HANDLE TPMI_SH_HMAC;              // (Part 2: Structures)
typedef TPM_HANDLE TPMI_SH_POLICY;            // (Part 2: Structures)
typedef TPM_HANDLE TPMI_DH_CONTEXT;           // (Part 2: Structures)
typedef TPM_HANDLE TPMI_DH_SAVED;             // (Part 2: Structures)
typedef TPM_HANDLE TPMI_RH_HIERARCHY;         // (Part 2: Structures)
typedef TPM_HANDLE TPMI_RH_ENABLES;           // (Part 2: Structures)
typedef TPM_HANDLE TPMI_RH_HIERARCHY_AUTH;    // (Part 2: Structures)
typedef TPM_HANDLE TPMI_RH_HIERARCHY_POLICY;  // (Part 2: Structures)
typedef TPM_HANDLE TPMI_RH_BASE_HIERARCHY;    // (Part 2: Structures)
typedef TPM_HANDLE TPMI_RH_PLATFORM;          // (Part 2: Structures)
typedef TPM_HANDLE TPMI_RH_OWNER;             // (Part 2: Structures)
typedef TPM_HANDLE TPMI_RH_ENDORSEMENT;       // (Part 2: Structures)
typedef TPM_HANDLE TPMI_RH_PROVISION;         // (Part 2: Structures)
typedef TPM_HANDLE TPMI_RH_CLEAR;             // (Part 2: Structures)
typedef TPM_HANDLE TPMI_RH_NV_AUTH;           // (Part 2: Structures)
typedef TPM_HANDLE TPMI_RH_LOCKOUT;           // (Part 2: Structures)
typedef TPM_HANDLE TPMI_RH_NV_INDEX;          // (Part 2: Structures)
typedef TPM_HANDLE TPMI_RH_NV_DEFINED_INDEX;  // (Part 2: Structures)
typedef TPM_HANDLE TPMI_RH_NV_LEGACY_INDEX;   // (Part 2: Structures)
typedef TPM_HANDLE TPMI_RH_NV_EXP_INDEX;      // (Part 2: Structures)
typedef TPM_HANDLE TPMI_RH_AC;                // (Part 2: Structures)
typedef TPM_HANDLE TPMI_RH_ACT;               // (Part 2: Structures)
typedef TPM_ALG_ID TPMI_ALG_HASH;             // (Part 2: Structures)
typedef TPM_ALG_ID TPMI_ALG_ASYM;             // (Part 2: Structures)
typedef TPM_ALG_ID TPMI_ALG_SYM;              // (Part 2: Structures)
typedef TPM_ALG_ID TPMI_ALG_SYM_OBJECT;       // (Part 2: Structures)
typedef TPM_ALG_ID TPMI_ALG_SYM_MODE;         // (Part 2: Structures)
typedef TPM_ALG_ID TPMI_ALG_KDF;              // (Part 2: Structures)
typedef TPM_ALG_ID TPMI_ALG_SIG_SCHEME;       // (Part 2: Structures)
typedef TPM_ALG_ID TPMI_ECC_KEY_EXCHANGE;     // (Part 2: Structures)
typedef TPM_ST     TPMI_ST_COMMAND_TAG;       // (Part 2: Structures)
typedef TPM_ALG_ID TPMI_ALG_MAC_SCHEME;       // (Part 2: Structures)
typedef TPM_ALG_ID TPMI_ALG_CIPHER_MODE;      // (Part 2: Structures)
typedef BYTE       TPMS_EMPTY;                // (Part 2: Structures)


// Table "Definition of Types for TPM2B_NONCE" (Part 2: Structures)
typedef TPM2B_DIGEST TPM2B_NONCE;
#define TYPE_OF_TPM2B_NONCE TPM2B_DIGEST

// Table "Definition of Types for TPM2B_AUTH" (Part 2: Structures)
typedef TPM2B_DIGEST TPM2B_AUTH;
#define TYPE_OF_TPM2B_AUTH TPM2B_DIGEST

// Table "Definition of Types for TPM2B_OPERAND" (Part 2: Structures)
typedef TPM2B_DIGEST TPM2B_OPERAND;
#define TYPE_OF_TPM2B_OPERAND TPM2B_DIGEST

//*** Other Types
// An AUTH_VALUE is a BYTE array containing a digest (TPMU_HA)
typedef BYTE AUTH_VALUE[sizeof(TPMU_HA)];

// A TIME_INFO is a BYTE array that can contain a TPMS_TIME_INFO
typedef BYTE TIME_INFO[sizeof(TPMS_TIME_INFO)];

// A NAME is a BYTE array that can contain a TPMU_NAME
typedef BYTE NAME[sizeof(TPMU_NAME)];

#define HASH_COUNT                                                                \
    (ALG_SHA1 + ALG_SHA256 + ALG_SHA384 + ALG_SHA512 + ALG_SM3_256 + ALG_SHA3_256 \
     + ALG_SHA3_384 + ALG_SHA3_512)

#define COMMAND_COUNT             (LIBRARY_COMMAND_ARRAY_SIZE + VENDOR_COMMAND_ARRAY_SIZE)

#define CC_CertifyX509 CC_NO

#define CC_AC_GetCapability CC_NO
#define CC_AC_Send          CC_NO

// The TPM reference implementation does not implement firmware upgrade.
#define CC_FieldUpgradeData  CC_NO
#define CC_FieldUpgradeStart CC_NO
#define CC_FirmwareRead      CC_NO

#define CC_YES YES
#define CC_NO  NO

// Normal commands:

#define CC_ACT_SetTimeout             (CC_YES && ACT_SUPPORT)
#define CC_ActivateCredential         CC_YES
#define CC_Certify                    CC_YES
#define CC_CertifyCreation            CC_YES
#define CC_ChangeEPS                  CC_YES
#define CC_ChangePPS                  CC_YES
#define CC_Clear                      CC_YES
#define CC_ClearControl               CC_YES
#define CC_ClockRateAdjust            CC_YES
#define CC_ClockSet                   CC_YES
#define CC_Commit                     (CC_YES && ALG_ECC)
#define CC_ContextLoad                CC_YES
#define CC_ContextSave                CC_YES
#define CC_Create                     CC_YES
#define CC_CreateLoaded               CC_YES
#define CC_CreatePrimary              CC_YES
#define CC_DictionaryAttackLockReset  CC_YES
#define CC_DictionaryAttackParameters CC_YES
#define CC_Duplicate                  CC_YES
#define CC_ECC_Decrypt                (CC_YES && ALG_ECC)
#define CC_ECC_Encrypt                (CC_YES && ALG_ECC)
#define CC_ECC_Parameters             (CC_YES && ALG_ECC)
#define CC_ECDH_KeyGen                (CC_YES && ALG_ECC)
#define CC_ECDH_ZGen                  (CC_YES && ALG_ECC)
#define CC_EC_Ephemeral               (CC_YES && ALG_ECC)
#define CC_EncryptDecrypt             CC_YES
#define CC_EncryptDecrypt2            CC_YES
#define CC_EventSequenceComplete      CC_YES
#define CC_EvictControl               CC_YES
#define CC_FlushContext               CC_YES
#define CC_GetCapability              CC_YES
#define CC_GetCommandAuditDigest      CC_YES
#define CC_GetRandom                  CC_YES
#define CC_GetSessionAuditDigest      CC_YES
#define CC_GetTestResult              CC_YES
#define CC_GetTime                    CC_YES
#define CC_HMAC                       (CC_YES && !ALG_CMAC)
#define CC_HMAC_Start                 (CC_YES && !ALG_CMAC)
#define CC_Hash                       CC_YES
#define CC_HashSequenceStart          CC_YES
#define CC_HierarchyChangeAuth        CC_YES
#define CC_HierarchyControl           CC_YES
#define CC_Import                     CC_YES
#define CC_IncrementalSelfTest        CC_YES
#define CC_Load                       CC_YES
#define CC_LoadExternal               CC_YES
#define CC_MAC                        (CC_YES && ALG_CMAC)
#define CC_MAC_Start                  (CC_YES && ALG_CMAC)
#define CC_MakeCredential             CC_YES
#define CC_NV_Certify                 CC_YES
#define CC_NV_ChangeAuth              CC_YES
#define CC_NV_DefineSpace             CC_YES
#define CC_NV_Extend                  CC_YES
#define CC_NV_GlobalWriteLock         CC_YES
#define CC_NV_Increment               CC_YES
#define CC_NV_Read                    CC_YES
#define CC_NV_ReadLock                CC_YES
#define CC_NV_ReadPublic              CC_YES
#define CC_NV_SetBits                 CC_YES
#define CC_NV_UndefineSpace           CC_YES
#define CC_NV_UndefineSpaceSpecial    CC_YES
#define CC_NV_Write                   CC_YES
#define CC_NV_WriteLock               CC_YES
#define CC_ObjectChangeAuth           CC_YES
#define CC_PCR_Allocate               CC_YES
#define CC_PCR_Event                  CC_YES
#define CC_PCR_Extend                 CC_YES
#define CC_PCR_Read                   CC_YES
#define CC_PCR_Reset                  CC_YES
#define CC_PCR_SetAuthPolicy          CC_YES
#define CC_PCR_SetAuthValue           CC_YES
#define CC_PP_Commands                CC_YES
#define CC_PolicyAuthValue            CC_YES
#define CC_PolicyAuthorize            CC_YES
#define CC_PolicyAuthorizeNV          CC_YES
#define CC_PolicyCapability           CC_YES
#define CC_PolicyCommandCode          CC_YES
#define CC_PolicyCounterTimer         CC_YES
#define CC_PolicyCpHash               CC_YES
#define CC_PolicyDuplicationSelect    CC_YES
#define CC_PolicyGetDigest            CC_YES
#define CC_PolicyLocality             CC_YES
#define CC_PolicyNV                   CC_YES
#define CC_PolicyNameHash             CC_YES
#define CC_PolicyNvWritten            CC_YES
#define CC_PolicyOR                   CC_YES
#define CC_PolicyPCR                  CC_YES
#define CC_PolicyPassword             CC_YES
#define CC_PolicyParameters           CC_YES
#define CC_PolicyPhysicalPresence     CC_YES
#define CC_PolicyRestart              CC_YES
#define CC_PolicySecret               CC_YES
#define CC_PolicySigned               CC_YES
#define CC_PolicyTemplate             CC_YES
#define CC_PolicyTicket               CC_YES
#define CC_Policy_AC_SendSelect       CC_YES
#define CC_Quote                      CC_YES
#define CC_RSA_Decrypt                (CC_YES && ALG_RSA)
#define CC_RSA_Encrypt                (CC_YES && ALG_RSA)
#define CC_ReadClock                  CC_YES
#define CC_ReadPublic                 CC_YES
#define CC_Rewrap                     CC_YES
#define CC_SelfTest                   CC_YES
#define CC_SequenceComplete           CC_YES
#define CC_SequenceUpdate             CC_YES
#define CC_SetAlgorithmSet            CC_YES
#define CC_SetCommandCodeAuditStatus  CC_YES
#define CC_SetPrimaryPolicy           CC_YES
#define CC_Shutdown                   CC_YES
#define CC_Sign                       CC_YES
#define CC_StartAuthSession           CC_YES
#define CC_Startup                    CC_YES
#define CC_StirRandom                 CC_YES
#define CC_TestParms                  CC_YES
#define CC_Unseal                     CC_YES
#define CC_Vendor_TCG_Test            CC_YES
#define CC_VerifySignature            CC_YES
#define CC_ZGen_2Phase                (CC_YES && ALG_ECC)
#define CC_NV_DefineSpace2            CC_YES
#define CC_NV_ReadPublic2             CC_YES
#define CC_SetCapability              CC_NO

#if COMPRESSED_LISTS
#  define ADD_FILL 0
#else
#  define ADD_FILL 1
#endif
#define LIBRARY_COMMAND_ARRAY_SIZE                                    \
    (0 + (ADD_FILL || CC_NV_UndefineSpaceSpecial)    /* 0x0000011F */ \
     + (ADD_FILL || CC_EvictControl)                 /* 0x00000120 */ \
     + (ADD_FILL || CC_HierarchyControl)             /* 0x00000121 */ \
     + (ADD_FILL || CC_NV_UndefineSpace)             /* 0x00000122 */ \
     + ADD_FILL                                      /* 0x00000123 */ \
     + (ADD_FILL || CC_ChangeEPS)                    /* 0x00000124 */ \
     + (ADD_FILL || CC_ChangePPS)                    /* 0x00000125 */ \
     + (ADD_FILL || CC_Clear)                        /* 0x00000126 */ \
     + (ADD_FILL || CC_ClearControl)                 /* 0x00000127 */ \
     + (ADD_FILL || CC_ClockSet)                     /* 0x00000128 */ \
     + (ADD_FILL || CC_HierarchyChangeAuth)          /* 0x00000129 */ \
     + (ADD_FILL || CC_NV_DefineSpace)               /* 0x0000012A */ \
     + (ADD_FILL || CC_PCR_Allocate)                 /* 0x0000012B */ \
     + (ADD_FILL || CC_PCR_SetAuthPolicy)            /* 0x0000012C */ \
     + (ADD_FILL || CC_PP_Commands)                  /* 0x0000012D */ \
     + (ADD_FILL || CC_SetPrimaryPolicy)             /* 0x0000012E */ \
     + (ADD_FILL || CC_FieldUpgradeStart)            /* 0x0000012F */ \
     + (ADD_FILL || CC_ClockRateAdjust)              /* 0x00000130 */ \
     + (ADD_FILL || CC_CreatePrimary)                /* 0x00000131 */ \
     + (ADD_FILL || CC_NV_GlobalWriteLock)           /* 0x00000132 */ \
     + (ADD_FILL || CC_GetCommandAuditDigest)        /* 0x00000133 */ \
     + (ADD_FILL || CC_NV_Increment)                 /* 0x00000134 */ \
     + (ADD_FILL || CC_NV_SetBits)                   /* 0x00000135 */ \
     + (ADD_FILL || CC_NV_Extend)                    /* 0x00000136 */ \
     + (ADD_FILL || CC_NV_Write)                     /* 0x00000137 */ \
     + (ADD_FILL || CC_NV_WriteLock)                 /* 0x00000138 */ \
     + (ADD_FILL || CC_DictionaryAttackLockReset)    /* 0x00000139 */ \
     + (ADD_FILL || CC_DictionaryAttackParameters)   /* 0x0000013A */ \
     + (ADD_FILL || CC_NV_ChangeAuth)                /* 0x0000013B */ \
     + (ADD_FILL || CC_PCR_Event)                    /* 0x0000013C */ \
     + (ADD_FILL || CC_PCR_Reset)                    /* 0x0000013D */ \
     + (ADD_FILL || CC_SequenceComplete)             /* 0x0000013E */ \
     + (ADD_FILL || CC_SetAlgorithmSet)              /* 0x0000013F */ \
     + (ADD_FILL || CC_SetCommandCodeAuditStatus)    /* 0x00000140 */ \
     + (ADD_FILL || CC_FieldUpgradeData)             /* 0x00000141 */ \
     + (ADD_FILL || CC_IncrementalSelfTest)          /* 0x00000142 */ \
     + (ADD_FILL || CC_SelfTest)                     /* 0x00000143 */ \
     + (ADD_FILL || CC_Startup)                      /* 0x00000144 */ \
     + (ADD_FILL || CC_Shutdown)                     /* 0x00000145 */ \
     + (ADD_FILL || CC_StirRandom)                   /* 0x00000146 */ \
     + (ADD_FILL || CC_ActivateCredential)           /* 0x00000147 */ \
     + (ADD_FILL || CC_Certify)                      /* 0x00000148 */ \
     + (ADD_FILL || CC_PolicyNV)                     /* 0x00000149 */ \
     + (ADD_FILL || CC_CertifyCreation)              /* 0x0000014A */ \
     + (ADD_FILL || CC_Duplicate)                    /* 0x0000014B */ \
     + (ADD_FILL || CC_GetTime)                      /* 0x0000014C */ \
     + (ADD_FILL || CC_GetSessionAuditDigest)        /* 0x0000014D */ \
     + (ADD_FILL || CC_NV_Read)                      /* 0x0000014E */ \
     + (ADD_FILL || CC_NV_ReadLock)                  /* 0x0000014F */ \
     + (ADD_FILL || CC_ObjectChangeAuth)             /* 0x00000150 */ \
     + (ADD_FILL || CC_PolicySecret)                 /* 0x00000151 */ \
     + (ADD_FILL || CC_Rewrap)                       /* 0x00000152 */ \
     + (ADD_FILL || CC_Create)                       /* 0x00000153 */ \
     + (ADD_FILL || CC_ECDH_ZGen)                    /* 0x00000154 */ \
     + (ADD_FILL || (CC_HMAC || CC_MAC))             /* 0x00000155 */ \
     + (ADD_FILL || CC_Import)                       /* 0x00000156 */ \
     + (ADD_FILL || CC_Load)                         /* 0x00000157 */ \
     + (ADD_FILL || CC_Quote)                        /* 0x00000158 */ \
     + (ADD_FILL || CC_RSA_Decrypt)                  /* 0x00000159 */ \
     + ADD_FILL                                      /* 0x0000015A */ \
     + (ADD_FILL || (CC_HMAC_Start || CC_MAC_Start)) /* 0x0000015B */ \
     + (ADD_FILL || CC_SequenceUpdate)               /* 0x0000015C */ \
     + (ADD_FILL || CC_Sign)                         /* 0x0000015D */ \
     + (ADD_FILL || CC_Unseal)                       /* 0x0000015E */ \
     + ADD_FILL                                      /* 0x0000015F */ \
     + (ADD_FILL || CC_PolicySigned)                 /* 0x00000160 */ \
     + (ADD_FILL || CC_ContextLoad)                  /* 0x00000161 */ \
     + (ADD_FILL || CC_ContextSave)                  /* 0x00000162 */ \
     + (ADD_FILL || CC_ECDH_KeyGen)                  /* 0x00000163 */ \
     + (ADD_FILL || CC_EncryptDecrypt)               /* 0x00000164 */ \
     + (ADD_FILL || CC_FlushContext)                 /* 0x00000165 */ \
     + ADD_FILL                                      /* 0x00000166 */ \
     + (ADD_FILL || CC_LoadExternal)                 /* 0x00000167 */ \
     + (ADD_FILL || CC_MakeCredential)               /* 0x00000168 */ \
     + (ADD_FILL || CC_NV_ReadPublic)                /* 0x00000169 */ \
     + (ADD_FILL || CC_PolicyAuthorize)              /* 0x0000016A */ \
     + (ADD_FILL || CC_PolicyAuthValue)              /* 0x0000016B */ \
     + (ADD_FILL || CC_PolicyCommandCode)            /* 0x0000016C */ \
     + (ADD_FILL || CC_PolicyCounterTimer)           /* 0x0000016D */ \
     + (ADD_FILL || CC_PolicyCpHash)                 /* 0x0000016E */ \
     + (ADD_FILL || CC_PolicyLocality)               /* 0x0000016F */ \
     + (ADD_FILL || CC_PolicyNameHash)               /* 0x00000170 */ \
     + (ADD_FILL || CC_PolicyOR)                     /* 0x00000171 */ \
     + (ADD_FILL || CC_PolicyTicket)                 /* 0x00000172 */ \
     + (ADD_FILL || CC_ReadPublic)                   /* 0x00000173 */ \
     + (ADD_FILL || CC_RSA_Encrypt)                  /* 0x00000174 */ \
     + ADD_FILL                                      /* 0x00000175 */ \
     + (ADD_FILL || CC_StartAuthSession)             /* 0x00000176 */ \
     + (ADD_FILL || CC_VerifySignature)              /* 0x00000177 */ \
     + (ADD_FILL || CC_ECC_Parameters)               /* 0x00000178 */ \
     + (ADD_FILL || CC_FirmwareRead)                 /* 0x00000179 */ \
     + (ADD_FILL || CC_GetCapability)                /* 0x0000017A */ \
     + (ADD_FILL || CC_GetRandom)                    /* 0x0000017B */ \
     + (ADD_FILL || CC_GetTestResult)                /* 0x0000017C */ \
     + (ADD_FILL || CC_Hash)                         /* 0x0000017D */ \
     + (ADD_FILL || CC_PCR_Read)                     /* 0x0000017E */ \
     + (ADD_FILL || CC_PolicyPCR)                    /* 0x0000017F */ \
     + (ADD_FILL || CC_PolicyRestart)                /* 0x00000180 */ \
     + (ADD_FILL || CC_ReadClock)                    /* 0x00000181 */ \
     + (ADD_FILL || CC_PCR_Extend)                   /* 0x00000182 */ \
     + (ADD_FILL || CC_PCR_SetAuthValue)             /* 0x00000183 */ \
     + (ADD_FILL || CC_NV_Certify)                   /* 0x00000184 */ \
     + (ADD_FILL || CC_EventSequenceComplete)        /* 0x00000185 */ \
     + (ADD_FILL || CC_HashSequenceStart)            /* 0x00000186 */ \
     + (ADD_FILL || CC_PolicyPhysicalPresence)       /* 0x00000187 */ \
     + (ADD_FILL || CC_PolicyDuplicationSelect)      /* 0x00000188 */ \
     + (ADD_FILL || CC_PolicyGetDigest)              /* 0x00000189 */ \
     + (ADD_FILL || CC_TestParms)                    /* 0x0000018A */ \
     + (ADD_FILL || CC_Commit)                       /* 0x0000018B */ \
     + (ADD_FILL || CC_PolicyPassword)               /* 0x0000018C */ \
     + (ADD_FILL || CC_ZGen_2Phase)                  /* 0x0000018D */ \
     + (ADD_FILL || CC_EC_Ephemeral)                 /* 0x0000018E */ \
     + (ADD_FILL || CC_PolicyNvWritten)              /* 0x0000018F */ \
     + (ADD_FILL || CC_PolicyTemplate)               /* 0x00000190 */ \
     + (ADD_FILL || CC_CreateLoaded)                 /* 0x00000191 */ \
     + (ADD_FILL || CC_PolicyAuthorizeNV)            /* 0x00000192 */ \
     + (ADD_FILL || CC_EncryptDecrypt2)              /* 0x00000193 */ \
     + (ADD_FILL || CC_AC_GetCapability)             /* 0x00000194 */ \
     + (ADD_FILL || CC_AC_Send)                      /* 0x00000195 */ \
     + (ADD_FILL || CC_Policy_AC_SendSelect)         /* 0x00000196 */ \
     + (ADD_FILL || CC_CertifyX509)                  /* 0x00000197 */ \
     + (ADD_FILL || CC_ACT_SetTimeout)               /* 0x00000198 */ \
     + (ADD_FILL || CC_ECC_Encrypt)                  /* 0x00000199 */ \
     + (ADD_FILL || CC_ECC_Decrypt)                  /* 0x0000019A */ \
     + (ADD_FILL || CC_PolicyCapability)             /* 0x0000019B */ \
     + (ADD_FILL || CC_PolicyParameters)             /* 0x0000019C */ \
     + (ADD_FILL || CC_NV_DefineSpace2)              /* 0x0000019D */ \
     + (ADD_FILL || CC_NV_ReadPublic2)               /* 0x0000019E */ \
     + (ADD_FILL || CC_SetCapability)                /* 0x0000019F */ \
    )
#define VENDOR_COMMAND_ARRAY_SIZE (CC_Vendor_TCG_Test)

typedef UINT16 TPM_ALG_ID;
#define TYPE_OF_TPM_ALG_ID       UINT16
#define ALG_ERROR_VALUE          0x0000
#define TPM_ALG_ERROR            (TPM_ALG_ID)(ALG_ERROR_VALUE)
#define ALG_RSA_VALUE            0x0001
#define TPM_ALG_RSA              (TPM_ALG_ID)(ALG_RSA_VALUE)
#define ALG_TDES_VALUE           0x0003
#define TPM_ALG_TDES             (TPM_ALG_ID)(ALG_TDES_VALUE)
#define ALG_SHA1_VALUE           0x0004
#define TPM_ALG_SHA1             (TPM_ALG_ID)(ALG_SHA1_VALUE)
#define ALG_HMAC_VALUE           0x0005
#define TPM_ALG_HMAC             (TPM_ALG_ID)(ALG_HMAC_VALUE)
#define ALG_AES_VALUE            0x0006
#define TPM_ALG_AES              (TPM_ALG_ID)(ALG_AES_VALUE)
#define ALG_MGF1_VALUE           0x0007
#define TPM_ALG_MGF1             (TPM_ALG_ID)(ALG_MGF1_VALUE)
#define ALG_KEYEDHASH_VALUE      0x0008
#define TPM_ALG_KEYEDHASH        (TPM_ALG_ID)(ALG_KEYEDHASH_VALUE)
#define ALG_XOR_VALUE            0x000A
#define TPM_ALG_XOR              (TPM_ALG_ID)(ALG_XOR_VALUE)
#define ALG_SHA256_VALUE         0x000B
#define TPM_ALG_SHA256           (TPM_ALG_ID)(ALG_SHA256_VALUE)
#define ALG_SHA384_VALUE         0x000C
#define TPM_ALG_SHA384           (TPM_ALG_ID)(ALG_SHA384_VALUE)
#define ALG_SHA512_VALUE         0x000D
#define TPM_ALG_SHA512           (TPM_ALG_ID)(ALG_SHA512_VALUE)
#define ALG_SHA256_192_VALUE     0x000E
#define TPM_ALG_SHA256_192       (TPM_ALG_ID)(ALG_SHA256_192_VALUE)
#define ALG_NULL_VALUE           0x0010
#define TPM_ALG_NULL             (TPM_ALG_ID)(ALG_NULL_VALUE)
#define ALG_SM3_256_VALUE        0x0012
#define TPM_ALG_SM3_256          (TPM_ALG_ID)(ALG_SM3_256_VALUE)
#define ALG_SM4_VALUE            0x0013
#define TPM_ALG_SM4              (TPM_ALG_ID)(ALG_SM4_VALUE)
#define ALG_RSASSA_VALUE         0x0014
#define TPM_ALG_RSASSA           (TPM_ALG_ID)(ALG_RSASSA_VALUE)
#define ALG_RSAES_VALUE          0x0015
#define TPM_ALG_RSAES            (TPM_ALG_ID)(ALG_RSAES_VALUE)
#define ALG_RSAPSS_VALUE         0x0016
#define TPM_ALG_RSAPSS           (TPM_ALG_ID)(ALG_RSAPSS_VALUE)
#define ALG_OAEP_VALUE           0x0017
#define TPM_ALG_OAEP             (TPM_ALG_ID)(ALG_OAEP_VALUE)
#define ALG_ECDSA_VALUE          0x0018
#define TPM_ALG_ECDSA            (TPM_ALG_ID)(ALG_ECDSA_VALUE)
#define ALG_ECDH_VALUE           0x0019
#define TPM_ALG_ECDH             (TPM_ALG_ID)(ALG_ECDH_VALUE)
#define ALG_ECDAA_VALUE          0x001A
#define TPM_ALG_ECDAA            (TPM_ALG_ID)(ALG_ECDAA_VALUE)
#define ALG_SM2_VALUE            0x001B
#define TPM_ALG_SM2              (TPM_ALG_ID)(ALG_SM2_VALUE)
#define ALG_ECSCHNORR_VALUE      0x001C
#define TPM_ALG_ECSCHNORR        (TPM_ALG_ID)(ALG_ECSCHNORR_VALUE)
#define ALG_ECMQV_VALUE          0x001D
#define TPM_ALG_ECMQV            (TPM_ALG_ID)(ALG_ECMQV_VALUE)
#define ALG_KDF1_SP800_56A_VALUE 0x0020
#define TPM_ALG_KDF1_SP800_56A   (TPM_ALG_ID)(ALG_KDF1_SP800_56A_VALUE)
#define ALG_KDF2_VALUE           0x0021
#define TPM_ALG_KDF2             (TPM_ALG_ID)(ALG_KDF2_VALUE)
#define ALG_KDF1_SP800_108_VALUE 0x0022
#define TPM_ALG_KDF1_SP800_108   (TPM_ALG_ID)(ALG_KDF1_SP800_108_VALUE)
#define ALG_ECC_VALUE            0x0023
#define TPM_ALG_ECC              (TPM_ALG_ID)(ALG_ECC_VALUE)
#define ALG_SYMCIPHER_VALUE      0x0025
#define TPM_ALG_SYMCIPHER        (TPM_ALG_ID)(ALG_SYMCIPHER_VALUE)
#define ALG_CAMELLIA_VALUE       0x0026
#define TPM_ALG_CAMELLIA         (TPM_ALG_ID)(ALG_CAMELLIA_VALUE)
#define ALG_SHA3_256_VALUE       0x0027
#define TPM_ALG_SHA3_256         (TPM_ALG_ID)(ALG_SHA3_256_VALUE)
#define ALG_SHA3_384_VALUE       0x0028
#define TPM_ALG_SHA3_384         (TPM_ALG_ID)(ALG_SHA3_384_VALUE)
#define ALG_SHA3_512_VALUE       0x0029
#define TPM_ALG_SHA3_512         (TPM_ALG_ID)(ALG_SHA3_512_VALUE)
#define ALG_SHAKE128_VALUE       0x002A
#define TPM_ALG_SHAKE128         (TPM_ALG_ID)(ALG_SHAKE128_VALUE)
#define ALG_SHAKE256_VALUE       0x002B
#define TPM_ALG_SHAKE256         (TPM_ALG_ID)(ALG_SHAKE256_VALUE)
#define ALG_SHAKE256_192_VALUE   0x002C
#define TPM_ALG_SHAKE256_192     (TPM_ALG_ID)(ALG_SHAKE256_192_VALUE)
#define ALG_SHAKE256_256_VALUE   0x002D
#define TPM_ALG_SHAKE256_256     (TPM_ALG_ID)(ALG_SHAKE256_256_VALUE)
#define ALG_SHAKE256_512_VALUE   0x002E
#define TPM_ALG_SHAKE256_512     (TPM_ALG_ID)(ALG_SHAKE256_512_VALUE)
#define ALG_CMAC_VALUE           0x003F
#define TPM_ALG_CMAC             (TPM_ALG_ID)(ALG_CMAC_VALUE)
#define ALG_CTR_VALUE            0x0040
#define TPM_ALG_CTR              (TPM_ALG_ID)(ALG_CTR_VALUE)
#define ALG_OFB_VALUE            0x0041
#define TPM_ALG_OFB              (TPM_ALG_ID)(ALG_OFB_VALUE)
#define ALG_CBC_VALUE            0x0042
#define TPM_ALG_CBC              (TPM_ALG_ID)(ALG_CBC_VALUE)
#define ALG_CFB_VALUE            0x0043
#define TPM_ALG_CFB              (TPM_ALG_ID)(ALG_CFB_VALUE)
#define ALG_ECB_VALUE            0x0044
#define TPM_ALG_ECB              (TPM_ALG_ID)(ALG_ECB_VALUE)
#define ALG_CCM_VALUE            0x0050
#define TPM_ALG_CCM              (TPM_ALG_ID)(ALG_CCM_VALUE)
#define ALG_GCM_VALUE            0x0051
#define TPM_ALG_GCM              (TPM_ALG_ID)(ALG_GCM_VALUE)
#define ALG_KW_VALUE             0x0052
#define TPM_ALG_KW               (TPM_ALG_ID)(ALG_KW_VALUE)
#define ALG_KWP_VALUE            0x0053
#define TPM_ALG_KWP              (TPM_ALG_ID)(ALG_KWP_VALUE)
#define ALG_EAX_VALUE            0x0054
#define TPM_ALG_EAX              (TPM_ALG_ID)(ALG_EAX_VALUE)
#define ALG_EDDSA_VALUE          0x0060
#define TPM_ALG_EDDSA            (TPM_ALG_ID)(ALG_EDDSA_VALUE)
#define ALG_EDDSA_PH_VALUE       0x0061
#define TPM_ALG_EDDSA_PH         (TPM_ALG_ID)(ALG_EDDSA_PH_VALUE)
#define ALG_LMS_VALUE            0x0070
#define TPM_ALG_LMS              (TPM_ALG_ID)(ALG_LMS_VALUE)
#define ALG_XMSS_VALUE           0x0071
#define TPM_ALG_XMSS             (TPM_ALG_ID)(ALG_XMSS_VALUE)
//        Values derived from Table "Definition of TPM_ALG_ID Constants" (Part 2: Structures)
#define ALG_FIRST_VALUE 0x0001
#define TPM_ALG_FIRST   (TPM_ALG_ID)(ALG_FIRST_VALUE)
#define ALG_LAST_VALUE  0x0071
#define TPM_ALG_LAST    (TPM_ALG_ID)(ALG_LAST_VALUE)

typedef struct PCR_POLICY
{
    TPMI_ALG_HASH hashAlg[NUM_POLICY_PCR_GROUP];
    TPM2B_DIGEST  a;
    TPM2B_DIGEST  policy[NUM_POLICY_PCR_GROUP];
} PCR_POLICY;


// Definition for a Primary Seed value
TPM2B_TYPE(SEED, 32);

TPM2B_TYPE(PROOF, 64);

typedef UINT16 TPM_SU;
#define TYPE_OF_TPM_SU UINT16
#define TPM_SU_CLEAR   (TPM_SU)(0x0000)
#define TPM_SU_STATE   (TPM_SU)(0x0001)

#define CLOCK_STOPS                 NO

#  if CLOCK_STOPS
typedef UINT64 CLOCK_NONCE;
#  else
typedef UINT32 CLOCK_NONCE;
#  endif

typedef struct
{
    // data provided by the platform library during manufacturing.
    // Opaque to the TPM Core library, but may be used by the platform library.
    BYTE platformReserved[PERSISTENT_DATA_PLATFORM_SPACE];

    //*********************************************************************************
    //          Hierarchy
    //*********************************************************************************
    // The values in this section are related to the hierarchies.

    BOOL disableClear;  // TRUE if TPM2_Clear() using
                        // lockoutAuth is disabled

    // Hierarchy authPolicies
    TPMI_ALG_HASH ownerAlg;
    TPMI_ALG_HASH endorsementAlg;
    TPMI_ALG_HASH lockoutAlg;
    TPM2B_DIGEST  ownerPolicy;
    TPM2B_DIGEST  endorsementPolicy;
    TPM2B_DIGEST  lockoutPolicy;

    // Hierarchy authValues
    TPM2B_AUTH ownerAuth;
    TPM2B_AUTH endorsementAuth;
    TPM2B_AUTH lockoutAuth;

    // Primary Seeds
    TPM2B_SEED EPSeed;
    TPM2B_SEED SPSeed;
    TPM2B_SEED PPSeed;
    // Note there is a nullSeed in the state_reset memory.

    // Hierarchy proofs
    TPM2B_PROOF phProof;
    TPM2B_PROOF shProof;
    TPM2B_PROOF ehProof;
    // Note there is a nullProof in the state_reset memory.

    //*********************************************************************************
    //          Reset Events
    //*********************************************************************************
    // A count that increments at each TPM reset and never get reset during the life
    // time of TPM.  The value of this counter is initialized to 1 during TPM
    // manufacture process. It is used to invalidate all saved contexts after a TPM
    // Reset.
    UINT64 totalResetCount;

    // This counter increments on each TPM Reset. The counter is reset by
    // TPM2_Clear().
    UINT32 resetCount;

//*********************************************************************************
//          PCR
//*********************************************************************************
// This structure hold the policies for those PCR that have an update policy.
// This implementation only supports a single group of PCR controlled by
// policy. If more are required, then this structure would be changed to
// an array.
#  if defined  NUM_POLICY_PCR_GROUP && NUM_POLICY_PCR_GROUP > 0
    PCR_POLICY pcrPolicies;
#  endif

    // This structure indicates the allocation of PCR. The structure contains a
    // list of PCR allocations for each implemented algorithm. If no PCR are
    // allocated for an algorithm, a list entry still exists but the bit map
    // will contain no SET bits.
    TPML_PCR_SELECTION pcrAllocated;

    //*********************************************************************************
    //          Physical Presence
    //*********************************************************************************
    // The PP_LIST type contains a bit map of the commands that require physical
    // to be asserted when the authorization is evaluated. Physical presence will be
    // checked if the corresponding bit in the array is SET and if the authorization
    // handle is TPM_RH_PLATFORM.
    //
    // These bits may be changed with TPM2_PP_Commands().
    BYTE ppList[(COMMAND_COUNT + 7) / 8];

    //*********************************************************************************
    //          Dictionary attack values
    //*********************************************************************************
    // These values are used for dictionary attack tracking and control.
    UINT32 failedTries;  // the current count of unexpired
                         // authorization failures

    UINT32 maxTries;  // number of unexpired authorization
                      // failures before the TPM is in
                      // lockout

    UINT32 recoveryTime;  // time between authorization failures
                          // before failedTries is decremented

    UINT32 lockoutRecovery;  // time that must expire between
                             // authorization failures associated
                             // with lockoutAuth

    BOOL lockOutAuthEnabled;  // TRUE if use of lockoutAuth is
                              // allowed

    //*****************************************************************************
    //            Orderly State
    //*****************************************************************************
    // The orderly state for current cycle
    TPM_SU orderlyState;

    //*****************************************************************************
    //           Command audit values.
    //*****************************************************************************
    BYTE          auditCommands[((COMMAND_COUNT + 1) + 7) / 8];
    TPMI_ALG_HASH auditHashAlg;
    UINT64        auditCounter;

    //*****************************************************************************
    //           Algorithm selection
    //*****************************************************************************
    //
    // The 'algorithmSet' value indicates the collection of algorithms that are
    // currently in used on the TPM.  The interpretation of value is vendor dependent.
    UINT32 algorithmSet;

    //*****************************************************************************
    //           Firmware version
    //*****************************************************************************
    // The firmwareV1 and firmwareV2 values are instanced in TimeStamp.c. This is
    // a scheme used in development to allow determination of the linker build time
    // of the TPM. An actual implementation would implement these values in a way that
    // is consistent with vendor needs. The values are maintained in RAM for simplified
    // access with a master version in NV.  These values are modified in a
    // vendor-specific way.

    // g_firmwareV1 contains the more significant 32-bits of the vendor version number.
    // In the reference implementation, if this value is printed as a hex
    // value, it will have the format of YYYYMMDD
    UINT32 firmwareV1;

    // g_firmwareV1 contains the less significant 32-bits of the vendor version number.
    // In the reference implementation, if this value is printed as a hex
    // value, it will have the format of 00 HH MM SS
    UINT32 firmwareV2;
//*****************************************************************************
//           Timer Epoch
//*****************************************************************************
// timeEpoch contains a nonce that has a vendor=specific size (should not be
// less than 8 bytes. This nonce changes when the clock epoch changes. The clock
// epoch changes when there is a discontinuity in the timing of the TPM.
#  if !CLOCK_STOPS
    CLOCK_NONCE timeEpoch;
#  endif

} PERSISTENT_DATA;

#define NV_IS_ORDERLY (IS_ORDERLY(gp.orderlyState))
extern PERSISTENT_DATA gp;



#endif