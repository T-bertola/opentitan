#include "TPM/TPMCmd/tpm/include/private/Tpm.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/FieldUpgradeStart_fp.h"
#if CC_FieldUpgradeStart  // Conditional expansion of this file

/*(See part 3 specification)
// FieldUpgradeStart
*/
TPM_RC
TPM2_FieldUpgradeStart(FieldUpgradeStart_In* in  // IN: input parameter list
)
{
    // Not implemented
    UNUSED_PARAMETER(in);
    return TPM_RC_SUCCESS;
}
#endif