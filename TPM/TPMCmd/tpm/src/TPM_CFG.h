#ifndef TPM_CFG_H
#define TPM_CFG_H

#include "sw/device/lib/base/mmio.h"
#include "hw/top_earlgrey/sw/autogen/top_earlgrey.h"
#include "sw/device/lib/dif/dif_rv_plic.h"
#include "sw/device/lib/dif/dif_otbn.h"
#include "sw/device/lib/runtime/irq.h"

//CSRNG
#include "csrng_regs.h"
#include "sw/device/lib/testing/csrng_testutils.h"
#include "sw/device/lib/testing/rand_testutils.h"
//Entropy Distribution Network 
#include "sw/device/lib/dif/dif_edn.h"
#include "sw/device/lib/dif/dif_entropy_src.h"
#include "sw/device/lib/testing/entropy_testutils.h"

typedef enum irq_flag_id {
  kTestIrqFlagIdCsrngEntropyReq,
  kTestIrqFlagIdEdn1CmdDone,
  kTestIrqFlagIdEdn0CmdDone,
  kTestIrqFlagCount,
} irq_flag_id_t;

extern dif_rv_plic_t plic;
extern volatile bool irq_flags[kTestIrqFlagCount];

//Function Definitions 
void init_CSRNG_peripherals(void);

void en_plic_irqs(void);

void plic_rng_interrupts_enable(void);

void CSRNG_ISR(dif_rv_plic_irq_id_t source, top_earlgrey_plic_peripheral_t peripheral_serviced);

#endif