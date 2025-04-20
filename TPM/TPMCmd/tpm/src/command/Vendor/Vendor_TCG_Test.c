#include "TPM/TPMCmd/tpm/include/private/Tpm.h"

#if CC_Vendor_TCG_Test  // Conditional expansion of this file
#include "TPM/TPMCmd/tpm/include/private/prototypes/Vendor_TCG_Test_fp.h"

TPM_RC
TPM2_Vendor_TCG_Test(Vendor_TCG_Test_In*  in,  // IN: input parameter list
                     Vendor_TCG_Test_Out* out  // OUT: output parameter list
)
{
    out->outputData = in->inputData;
    return TPM_RC_SUCCESS;
}

#endif  // CC_Vendor_TCG_Test