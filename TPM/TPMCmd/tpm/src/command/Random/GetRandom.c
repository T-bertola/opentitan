#include "TPM/TPMCmd/tpm/include/private/Tpm.h"
#include "TPM/TPMCmd/tpm/include/private/prototypes/GetRandom_fp.h"
#include "sw/device/lib/runtime/log.h"
#include "csrng_regs.h"
#include "sw/device/lib/testing/csrng_testutils.h"
#include "sw/device/lib/testing/test_framework/check.h"
#include "TPM/TPMCmd/tpm/src/TPM_CFG.h"


#if CC_GetRandom  // Conditional expansion of this file

static dif_csrng_t csrng;
static dif_edn_t edn0;
static dif_edn_t edn1;
static dif_entropy_src_t entropy_src;
static dif_otbn_t otbn;

static bool First = false;

volatile bool irq_flags[kTestIrqFlagCount];

dif_edn_seed_material_t edn_seed;

dif_csrng_seed_material_t csrng_seed;

void plic_rng_interrupts_enable(void) {

  CHECK_DIF_OK(dif_csrng_irq_set_enabled(&csrng, kDifCsrngIrqCsEntropyReq,
                                         kDifToggleEnabled));
  CHECK_DIF_OK(dif_edn_irq_set_enabled(&edn0, kDifEdnIrqEdnCmdReqDone,
                                       kDifToggleEnabled));
  CHECK_DIF_OK(dif_edn_irq_set_enabled(&edn1, kDifEdnIrqEdnCmdReqDone,
                                       kDifToggleEnabled));

}

void CSRNG_ISR(dif_rv_plic_irq_id_t source, top_earlgrey_plic_peripheral_t peripheral_serviced){
            if (peripheral_serviced == kTopEarlgreyPlicPeripheralCsrng) {
            dif_csrng_irq_t irq =
                (dif_csrng_irq_t)(source - kTopEarlgreyPlicIrqIdCsrngCsCmdReqDone);
            CHECK(irq == kDifCsrngIrqCsEntropyReq, "Unexpected irq: 0x%x", irq);
            CHECK_DIF_OK(dif_csrng_irq_acknowledge(&csrng, irq));
            irq_flags[kTestIrqFlagIdCsrngEntropyReq] = true;
        } else if (peripheral_serviced == kTopEarlgreyPlicPeripheralEdn0) {
            dif_edn_irq_t irq =
                (dif_edn_irq_t)(source - kTopEarlgreyPlicIrqIdEdn0EdnCmdReqDone);
            CHECK(irq == kDifEdnIrqEdnCmdReqDone, "Unexpected irq: 0x%x", irq);
            CHECK_DIF_OK(dif_edn_irq_acknowledge(&edn0, irq));
            irq_flags[kTestIrqFlagIdEdn0CmdDone] = true;
        } else if (peripheral_serviced == kTopEarlgreyPlicPeripheralEdn1) {
            dif_edn_irq_t irq =
                (dif_edn_irq_t)(source - kTopEarlgreyPlicIrqIdEdn1EdnCmdReqDone);
            CHECK(irq == kDifEdnIrqEdnCmdReqDone, "Unexpected irq: 0x%x", irq);
            CHECK_DIF_OK(dif_edn_irq_acknowledge(&edn1, irq));
            irq_flags[kTestIrqFlagIdEdn1CmdDone] = true;
        }
}

void init_CSRNG_peripherals(void) {
    //LOG_INFO("Inititalizing the CSRNG");
  CHECK_DIF_OK(dif_csrng_init(
      mmio_region_from_addr(TOP_EARLGREY_CSRNG_BASE_ADDR), &csrng));
  CHECK_DIF_OK(
      dif_edn_init(mmio_region_from_addr(TOP_EARLGREY_EDN0_BASE_ADDR), &edn0));
  CHECK_DIF_OK(
      dif_edn_init(mmio_region_from_addr(TOP_EARLGREY_EDN1_BASE_ADDR), &edn1));
  CHECK_DIF_OK(dif_entropy_src_init(
      mmio_region_from_addr(TOP_EARLGREY_ENTROPY_SRC_BASE_ADDR), &entropy_src));
  CHECK_DIF_OK(
      dif_otbn_init(mmio_region_from_addr(TOP_EARLGREY_OTBN_BASE_ADDR), &otbn));

       // Get test random parameters before we disable the entropy complex.
    // rand_testutils relies on the ibex rnd CSR which is connected to EDN0.
    csrng_seed.seed_material_len =
        rand_testutils_gen32_range(/*min=*/0, kDifCsrngSeedMaterialMaxWordLen);
    for (size_t i = 0; i < csrng_seed.seed_material_len; ++i) {
        csrng_seed.seed_material[i] = rand_testutils_gen32();
    }

    edn_seed.len =
        rand_testutils_gen32_range(/*min=*/0, kDifEntropySeedMaterialMaxWordLen);
    for (size_t i = 0; i < edn_seed.len; ++i) {
        edn_seed.data[i] = rand_testutils_gen32();
    }

    CHECK_STATUS_OK(entropy_testutils_stop_all());
    CHECK_DIF_OK(dif_entropy_src_configure(
    &entropy_src, entropy_testutils_config_default(), kDifToggleEnabled));
    CHECK_DIF_OK(dif_csrng_configure(&csrng));
    CHECK_DIF_OK(dif_csrng_clear_recoverable_alerts(&csrng));
    CHECK_STATUS_OK(csrng_testutils_cmd_ready_wait(&csrng));
    
    //LOG_INFO("CSRNG and EDN Initialized with seeds %08x and %08x", edn_seed.data[0], csrng_seed.seed_material[0]);
}

static void irq_block_wait(irq_flag_id_t isr_id) {
  ATOMIC_WAIT_FOR_INTERRUPT(irq_flags[isr_id]);
  LOG_INFO("GOT %d Interrupt", isr_id);
  switch (isr_id) {
    case kTestIrqFlagIdCsrngEntropyReq:
      CHECK_DIF_OK(dif_csrng_irq_set_enabled(&csrng, kDifCsrngIrqCsEntropyReq,
                                             kDifToggleDisabled));
      break;
    case kTestIrqFlagIdEdn0CmdDone:
      CHECK_DIF_OK(dif_edn_irq_set_enabled(&edn0, kDifEdnIrqEdnCmdReqDone,
                                           kDifToggleDisabled));
      break;
    case kTestIrqFlagIdEdn1CmdDone:
      CHECK_DIF_OK(dif_edn_irq_set_enabled(&edn0, kDifEdnIrqEdnCmdReqDone,
                                           kDifToggleDisabled));
      break;
    default:
      CHECK(false, "Invalid isr_id: %d", isr_id);
  }
}

/*(See part 3 specification)
// random number generator
*/
TPM_RC
TPM2_GetRandom(GetRandom_In*  in,  // IN: input parameter list
               GetRandom_Out* out  // OUT: output parameter list
)
{
    // Command Output
    LOG_INFO("DEBUG INFO: IN GET_RANDOM FUNCTION");
    // if the requested bytes exceed the output buffer size, generates the
    // maximum bytes that the output buffer allows
    plic_rng_interrupts_enable();
    // en_plic_irqs();
    if (First == false){
        CHECK_DIF_OK(dif_csrng_instantiate(&csrng, kDifCsrngEntropySrcToggleEnable,
                                    &csrng_seed));
        First = true;
    }else{
        CHECK_DIF_OK(dif_csrng_reseed(&csrng, &csrng_seed));
    }

    irq_block_wait(kTestIrqFlagIdCsrngEntropyReq);

    if(in->bytesRequested > sizeof(TPMU_HA))
        out->randomBytes.t.size = sizeof(TPMU_HA);
    else
        out->randomBytes.t.size = in->bytesRequested;

          uint32_t output[12] = {0};
    CHECK_STATUS_OK(
      csrng_testutils_cmd_generate_run(&csrng, output, ARRAYSIZE(output)));

    LOG_INFO("RANDOM NUMBER %8x", output[0]);

    CHECK_STATUS_OK(csrng_testutils_cmd_status_check(&csrng));
    CHECK_STATUS_OK(csrng_testutils_recoverable_alerts_check(&csrng));

    memcpy(out->randomBytes.t.buffer, output,out->randomBytes.t.size );

    //   for(int i = 0; i < out->randomBytes.t.size; i++){
    // LOG_INFO("DEBUG INFO: BUF[%d] = %02x", i, out->randomBytes.t.buffer[i]);
    // }

    plic_rng_interrupts_enable();

    return TPM_RC_SUCCESS;
}

#endif  // CC_GetRandom