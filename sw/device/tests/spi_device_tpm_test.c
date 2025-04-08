// Copyright lowRISC contributors (OpenTitan project).
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include "TPM/TPMCmd/tpm/include/private/Tpm.h"
#include "TPM/TPMCmd/tpm/include/private/Marshal.h"
#include "TPM/TPMCmd/Platform/include/Platform.h"
// TODO_RENAME_INC_FOLDER:platform_interface refers to the TPM_CoreLib platform interface
#include "TPM/TPMCmd/tpm/include/platform_interface/prototypes/ExecCommand_fp.h"

#include "sw/device/lib/arch/device.h"
#include "sw/device/lib/base/mmio.h"
#include "sw/device/lib/dif/dif_base.h"
#include "sw/device/lib/dif/dif_pinmux.h"
#include "sw/device/lib/dif/dif_rv_plic.h"
#include "sw/device/lib/dif/dif_spi_device.h"
#include "sw/device/lib/runtime/hart.h"
#include "sw/device/lib/runtime/irq.h"
#include "sw/device/lib/runtime/log.h"
#include "sw/device/lib/testing/spi_device_testutils.h"
#include "sw/device/lib/testing/test_framework/check.h"
#include "sw/device/lib/testing/test_framework/ottf_main.h"
#include "sw/device/lib/testing/test_framework/status.h"
#include "spi_device_regs.h"
#include "TPM/TPMCmd/tpm/src/TPM_CFG.h"
#include "pinmux_regs.h"

#include "hw/top_earlgrey/sw/autogen/top_earlgrey.h"
#include "sw/device/lib/testing/autogen/isr_testutils.h"

#define DEBUG_PRINTS

#define ENABLE_OSSL

OTTF_DEFINE_TEST_CONFIG();

static dif_spi_device_handle_t spi_device;
static dif_pinmux_t pinmux;
dif_rv_plic_t plic;


typedef unsigned char* _OUTPUT_BUFFER;

typedef struct out_buffer
{
    uint32_t       BufferSize;
    _OUTPUT_BUFFER Buffer;
} _OUT_BUFFER;

// Enum for TPM command
typedef enum {
  kTpmWriteCommand = 0x0,
  kTpmReadCommand = 0x80,
  kTpmCommandMask = 0xbf
} tpm_cmd_t;

enum {
  kIterations = 10,
  kTpmCommandRwMask = 0x80,
  kTpmCommandSizeMask = 0x3f,
};

const static dif_spi_device_tpm_config_t kTpmConfig = {
    .interface = kDifSpiDeviceTpmInterfaceCrb,
    .disable_return_by_hardware = false,
    .disable_address_prefix_check = false,
    .disable_locality_check = false};

static volatile bool header_interrupt_received = false;

void __wrap_abort(void) {
  // Just call your OpenTitan version directly
  extern void abort(void);
  abort();
}

void bytes_to_words_util(const uint8_t *bytes, uint32_t byte_count, uint32_t *output, uint32_t *output_count) {
    uint32_t i, j;
    *output_count = 0; // Initialize the output count
    for (i = 0; i < byte_count; i += 4) {
        uint32_t compressed = 0;
        for (j = 0; j < 4 && (i + j) < byte_count; ++j) {
            compressed |= ((uint32_t)bytes[i + j]) << (8 * (3 - j)); // Place bytes in the correct position
        }
        output[*output_count] = compressed;
        (*output_count)++;
    }
}

void en_plic_irqs(void) {

  // Enable functional interrupts as well as error interrupts to make sure
  // everything is behaving as expected.
  const top_earlgrey_plic_irq_id_t kIrqs[] = {
      kTopEarlgreyPlicIrqIdSpiDeviceUploadCmdfifoNotEmpty,
      kTopEarlgreyPlicIrqIdSpiDeviceUploadPayloadNotEmpty,
      kTopEarlgreyPlicIrqIdSpiDeviceUploadPayloadOverflow,
      kTopEarlgreyPlicIrqIdSpiDeviceReadbufWatermark,
      kTopEarlgreyPlicIrqIdSpiDeviceReadbufFlip,
      kTopEarlgreyPlicIrqIdSpiDeviceTpmHeaderNotEmpty,
      kTopEarlgreyPlicIrqIdCsrngCsEntropyReq,};

  for (uint32_t i = 0; i < ARRAYSIZE(kIrqs); ++i) {
    CHECK_DIF_OK(dif_rv_plic_irq_set_enabled(
        &plic, kIrqs[i], kTopEarlgreyPlicTargetIbex0, kDifToggleEnabled));

    // Assign a default priority
    CHECK_DIF_OK(
        dif_rv_plic_irq_set_priority(&plic, kIrqs[i], kDifRvPlicMaxPriority));
  }

  // Enable the external IRQ at Ibex.
  irq_global_ctrl(true);
  irq_external_ctrl(true);
}

static void en_spi_device_irqs(dif_spi_device_t *spi_device) {
  const dif_spi_device_irq_t kIrqs[] = {kDifSpiDeviceIrqUploadCmdfifoNotEmpty,
                                        kDifSpiDeviceIrqUploadPayloadNotEmpty,
                                        kDifSpiDeviceIrqUploadPayloadOverflow,
                                        kDifSpiDeviceIrqReadbufWatermark,
                                        kDifSpiDeviceIrqReadbufFlip,
                                        kDifSpiDeviceIrqTpmHeaderNotEmpty};

  for (uint32_t i = 0; i <= ARRAYSIZE(kIrqs); ++i) {
    CHECK_DIF_OK(dif_spi_device_irq_set_enabled(spi_device, kIrqs[i],
                                                kDifToggleEnabled));
  }
}

void spi_device_isr(
    plic_isr_ctx_t plic_ctx, spi_device_isr_ctx_t spi_device_ctx,
    bool mute_status_irq, top_earlgrey_plic_peripheral_t *peripheral_serviced,
    dif_spi_device_irq_t *irq_serviced, dif_rv_plic_irq_id_t plic_irq_id){

  // Get the peripheral the IRQ belongs to.
  *peripheral_serviced = (top_earlgrey_plic_peripheral_t)
      top_earlgrey_plic_interrupt_for_peripheral[plic_irq_id];

  // Get the IRQ that was fired from the PLIC IRQ ID.
  dif_spi_device_irq_t irq =
      (dif_spi_device_irq_t)(plic_irq_id -
                             spi_device_ctx.plic_spi_device_start_irq_id);
  *irq_serviced = irq;

  // Check if it is supposed to be the only IRQ fired.
  if (spi_device_ctx.is_only_irq) {
    dif_spi_device_irq_state_snapshot_t snapshot;
    CHECK_DIF_OK(
        dif_spi_device_irq_get_state(spi_device_ctx.spi_device, &snapshot));
    CHECK(snapshot == (dif_spi_device_irq_state_snapshot_t)(1 << irq),
          "Only spi_device IRQ %d expected to fire. Actual IRQ state = %x", irq,
          snapshot);
  }

  // Acknowledge the IRQ at the peripheral if IRQ is of the event type.
  dif_irq_type_t type;
  CHECK_DIF_OK(
      dif_spi_device_irq_get_type(spi_device_ctx.spi_device, irq, &type));
  if (type == kDifIrqTypeEvent) {
    CHECK_DIF_OK(
        dif_spi_device_irq_acknowledge(spi_device_ctx.spi_device, irq));
  } else if (mute_status_irq) {
    CHECK_DIF_OK(dif_spi_device_irq_set_enabled(spi_device_ctx.spi_device, irq,
                                                kDifToggleDisabled));
  }

  // Complete the IRQ at the PLIC.
  CHECK_DIF_OK(dif_rv_plic_irq_complete(plic_ctx.rv_plic, plic_ctx.hart_id,
                                        plic_irq_id));
}


void ottf_external_isr(uint32_t *exc_info) {
  plic_isr_ctx_t plic_ctx = {.rv_plic = &plic,
                             .hart_id = kTopEarlgreyPlicTargetIbex0};

  top_earlgrey_plic_peripheral_t peripheral;
  dif_spi_device_irq_t spi_device_irq;
  spi_device_isr_ctx_t spi_device_ctx = {
      .spi_device = &spi_device.dev,
      .plic_spi_device_start_irq_id = kTopEarlgreyPlicIrqIdSpiDeviceUploadCmdfifoNotEmpty,
      .expected_irq = kDifSpiDeviceIrqTpmHeaderNotEmpty,
      .is_only_irq = false};

  dif_rv_plic_target_t target = 0;
  dif_rv_plic_irq_id_t source;
  CHECK_DIF_OK(dif_rv_plic_irq_claim(&plic, target, &source));
    top_earlgrey_plic_peripheral_t peripheral_serviced =
      (top_earlgrey_plic_peripheral_t)
          top_earlgrey_plic_interrupt_for_peripheral[source];
  //LOG_INFO("IN ISR: Source: %d for Peripheral: %d", source, peripheral_serviced);

  switch (source) {
    case kTopEarlgreyPlicIrqIdSpiDeviceTpmHeaderNotEmpty:
      header_interrupt_received = true;
      spi_device_isr(plic_ctx, spi_device_ctx, false, &peripheral,
                              &spi_device_irq, source);
      // Disable interrupt until work is handled.
      CHECK_DIF_OK(dif_spi_device_irq_set_enabled(
          &spi_device.dev, kDifSpiDeviceIrqTpmHeaderNotEmpty,
          kDifToggleDisabled));
      break;

    case kTopEarlgreyPlicIrqIdCsrngCsEntropyReq:
    case kTopEarlgreyPlicIrqIdCsrngCsCmdReqDone:
    case kTopEarlgreyPlicIrqIdCsrngCsHwInstExc:
    case kTopEarlgreyPlicIrqIdCsrngCsFatalErr:
        CSRNG_ISR(source, peripheral_serviced);
      break;
    default:
      LOG_ERROR("Unexpected interrupt: %d", source);
      break;
  }
}

static void ack_spi_tpm_header_irq(dif_spi_device_handle_t *spi_device) {
  // Clear interrupt state and re-enable interrupt.
  header_interrupt_received = false;
  CHECK_DIF_OK(dif_spi_device_irq_acknowledge(
      &spi_device->dev, kDifSpiDeviceIrqTpmHeaderNotEmpty));
  CHECK_DIF_OK(dif_spi_device_irq_set_enabled(
      &spi_device->dev, kDifSpiDeviceIrqTpmHeaderNotEmpty, kDifToggleEnabled));
}

LIB_EXPORT void ExecuteCommand(
    uint32_t        requestSize,   // IN: command buffer size
    unsigned char*  request,       // IN: command buffer
    uint32_t*       responseSize,  // IN/OUT: response buffer size
    unsigned char** response       // IN/OUT: response buffer
)
{
    // Command local variables
    UINT32  commandSize;
    COMMAND command;

    // Response local variables
    UINT32 maxResponse = *responseSize;
    TPM_RC result;  // return code for the command

    // This next function call is used in development to size the command and response
    // buffers. The values printed are the sizes of the internal structures and
    // not the sizes of the canonical forms of the command response structures. Also,
    // the sizes do not include the tag, command.code, requestSize, or the authorization
    // fields.
    //CommandResponseSizes();
    // Set flags for NV access state. This should happen before any other
    // operation that may require a NV write. Note, that this needs to be done
    // even when in failure mode. Otherwise, g_updateNV would stay SET while in
    // Failure mode and the NV would be written on each call.
    g_updateNV     = UT_NONE;
    g_clearOrderly = FALSE;

    if(g_inFailureMode)
    {
        // Do failure mode processing
        #ifdef ENABLE_OSSL
        LOG_INFO("DEBUG INFO: g_updateNV = %d, g_clearOrderly = %d, g_inFailureMode = %d", g_updateNV, g_clearOrderly, g_inFailureMode);
        TpmFailureMode(requestSize, request, responseSize, response);
        #endif
        return;
    }
    // Query platform to get the NV state.  The result state is saved internally
    // and will be reported by NvIsAvailable(). The reference code requires that
    // accessibility of NV does not change during the execution of a command.
    // Specifically, if NV is available when the command execution starts and then
    // is not available later when it is necessary to write to NV, then the TPM
    // will go into failure mode.
    NvCheckState();

    // Due to the limitations of the simulation, TPM clock must be explicitly
    // synchronized with the system clock whenever a command is received.
    // This function call is not necessary in a hardware TPM. However, taking
    // a snapshot of the hardware timer at the beginning of the command allows
    // the time value to be consistent for the duration of the command execution.
    #ifdef ENABLE_OSSL
    TimeUpdateToCurrent();
    #endif

    // Any command through this function will unceremoniously end the
    // _TPM_Hash_Data/_TPM_Hash_End sequence.

    if(g_DRTMHandle != TPM_RH_UNASSIGNED){
        LOG_INFO("DEBUG INFO: g_DRTMHandle = %d", g_DRTMHandle);
        #ifdef ENABLE_OSSL
        ObjectTerminateEvent();
        #endif
    }

    // Get command buffer size and command buffer.
    command.parameterBuffer = request;
    command.parameterSize   = requestSize;

    // Parse command header: tag, commandSize and command.code.
    // First parse the tag. The unmarshaling routine will validate
    // that it is either TPM_ST_SESSIONS or TPM_ST_NO_SESSIONS.
    result = TPMI_ST_COMMAND_TAG_Unmarshal(
        &command.tag, &command.parameterBuffer, &command.parameterSize);

    if(result != TPM_RC_SUCCESS){
        LOG_INFO("DEBUG INFO: TAG = 0x%04x, ParameterSize = %d, Result = %d ", command.tag,  command.parameterSize, result);
        goto Cleanup;
    }
    // Unmarshal the commandSize indicator.
    result = UINT32_Unmarshal(
        &commandSize, &command.parameterBuffer, &command.parameterSize);

    if(result != TPM_RC_SUCCESS){
        LOG_INFO("DEBUG INFO: CommandSize = 0x%04x, Result = %d ", commandSize, result);
        goto Cleanup;
    }
    // On a TPM that receives bytes on a port, the number of bytes that were
    // received on that port is requestSize it must be identical to commandSize.
    // In addition, commandSize must not be larger than MAX_COMMAND_SIZE allowed
    // by the implementation. The check against MAX_COMMAND_SIZE may be redundant
    // as the input processing (the function that receives the command bytes and
    // places them in the input buffer) would likely have the input truncated when
    // it reaches MAX_COMMAND_SIZE, and requestSize would not equal commandSize.
   
    if(commandSize != requestSize || commandSize > MAX_COMMAND_SIZE)
    {
        LOG_INFO("DEBUG INFO: CommandSize = 0x%04x, RequestRize = %d, MAX_CMD_SIZE = %d", commandSize, requestSize, MAX_COMMAND_SIZE);
        result = TPM_RC_COMMAND_SIZE;
        goto Cleanup;
    }
    // Unmarshal the command code.
    result = TPM_CC_Unmarshal(
        &command.code, &command.parameterBuffer, &command.parameterSize);
    if(result != TPM_RC_SUCCESS){
        LOG_INFO("DEBUG INFO: Command Code = 0x%08x, Result = %d ", command.code, result);
        goto Cleanup;
    }
    // Check to see if the command is implemented.
    command.index = CommandCodeToCommandIndex(command.code);
    if(UNIMPLEMENTED_COMMAND_INDEX == command.index)
    {
        LOG_INFO("DEBUG INFO: Command Code Index = 0x%08x", command.index);
        result = TPM_RC_COMMAND_CODE;
        goto Cleanup;
    }
#if FIELD_UPGRADE_IMPLEMENTED == YES
    // If the TPM is in FUM, then the only allowed command is
    // TPM_CC_FieldUpgradeData.
    if(IsFieldUgradeMode() && (command.code != TPM_CC_FieldUpgradeData))
    {
        result = TPM_RC_UPGRADE;
        goto Cleanup;
    }
    else
#endif
        // Excepting FUM, the TPM only accepts TPM2_Startup() after
        // _TPM_Init. After getting a TPM2_Startup(), TPM2_Startup()
        // is no longer allowed.
        #ifdef DEBUG_PRINT 
        LOG_INFO("DEBUG INFO: TPM Started = %d", TPMIsStarted());
        #endif 
    //COMMENTING THIS OUT FOR NOW   
        // if((!TPMIsStarted() && command.code != TPM_CC_Startup)
        //    || (TPMIsStarted() && command.code == TPM_CC_Startup))
        // {
        //     result = TPM_RC_INITIALIZE;
        //     goto Cleanup;
        // }
    // Start regular command process.
    #ifdef ENABLE_OSSL
    NvIndexCacheInit();
    #endif
    // Parse Handle buffer.
    //#ifdef ENABLE_OSSL
    result = ParseHandleBuffer(&command);

    //#endif
    if(result != TPM_RC_SUCCESS){
        LOG_INFO("DEBUG INFO: Parse Handle Result = %d", result);
        goto Cleanup;
    }
    // All handles in the handle area are required to reference TPM-resident
    // entities.
    #ifdef ENABLE_OSSL
   // result = EntityGetLoadStatus(&command);
    #endif
    if(result != TPM_RC_SUCCESS){
        LOG_INFO("Entity Load Result: %d", result);
        goto Cleanup;
    }
    // Authorization session handling for the command.
    #ifdef ENABLE_OSSL
    ClearCpRpHashes(&command);
    #endif
    if(command.tag == TPM_ST_SESSIONS)
    {
        // Find out session buffer size.
        result = UINT32_Unmarshal((UINT32*)&command.authSize,
                                  &command.parameterBuffer,
                                  &command.parameterSize);
        if(result != TPM_RC_SUCCESS){
            LOG_INFO("UnMarshal %d", result);
            goto Cleanup;
        }    
        // Perform sanity check on the unmarshaled value. If it is smaller than
        // the smallest possible session or larger than the remaining size of
        // the command, then it is an error. NOTE: This check could pass but the
        // session size could still be wrong. That will be determined after the
        // sessions are unmarshaled.
        if(command.authSize < 9 || command.authSize > command.parameterSize)
        {
            LOG_INFO("authsize: %08x, paramtereSize %08x", command.authSize,command.parameterSize );
            result = TPM_RC_SIZE;
            goto Cleanup;
        }
        command.parameterSize -= command.authSize;

        // The actions of ParseSessionBuffer() are described in the introduction.
        // As the sessions are parsed command.parameterBuffer is advanced so, on a
        // successful return, command.parameterBuffer should be pointing at the
        // first byte of the parameters.
        //#ifdef ENABLE_OSSL
        result = ParseSessionBuffer(&command);

        //#endif
        if(result != TPM_RC_SUCCESS){
            LOG_INFO("DEBUG INFO: Parse Session Result = %d", result);
            goto Cleanup;
        }
    }
    else
    {
        command.authSize = 0;
        // The command has no authorization sessions.
        // If the command requires authorizations, then CheckAuthNoSession() will
        // return an error.
        //#ifdef ENABLE_OSSL
        result = CheckAuthNoSession(&command);

        if(result != TPM_RC_SUCCESS){
            LOG_INFO("DEBUG INFO: CheckAuthSession Result = %d", result);
            goto Cleanup;
        }
    }
    // Set up the response buffer pointers. CommandDispatch will marshal the
    // response parameters starting at the address in command.responseBuffer.
    //*response = MemoryGetResponseBuffer(command.index);
    // leave space for the command header
    command.responseBuffer = *response + STD_RESPONSE_HEADER;

    // leave space for the parameter size field if needed
    if(command.tag == TPM_ST_SESSIONS)
        command.responseBuffer += sizeof(UINT32);
    if(IsHandleInResponse(command.index))
        command.responseBuffer += sizeof(TPM_HANDLE);

    // CommandDispatcher returns a response handle buffer and a response parameter
    // buffer if it succeeds. It will also set the parameterSize field in the
    // buffer if the tag is TPM_RC_SESSIONS.
    result = CommandDispatcher(&command);
    
    if(result != TPM_RC_SUCCESS){
        LOG_INFO("DEBUG INFO: Command Dispatch Result = %d", result);
        goto Cleanup;
    }
    // Build the session area at the end of the parameter area.
    result = BuildResponseSession(&command);
    
    if(result != TPM_RC_SUCCESS)
    {
        LOG_INFO("DEBUG INFO: BuildResponseSession Result = %d", result);
        goto Cleanup;
    }

Cleanup:
    if(result != TPM_RC_SUCCESS){
        LOG_INFO("RESULT Cleanup: %d", result);
    }
    if(g_clearOrderly == TRUE && NV_IS_ORDERLY)
    {
#if USE_DA_USED
        gp.orderlyState = g_daUsed ? SU_DA_USED_VALUE : SU_NONE_VALUE;
#else
        gp.orderlyState = SU_NONE_VALUE;
#endif
        NV_SYNC_PERSISTENT(orderlyState);
    }
    // This implementation loads an "evict" object to a transient object slot in
    // RAM whenever an "evict" object handle is used in a command so that the
    // access to any object is the same. These temporary objects need to be
    // cleared from RAM whether the command succeeds or fails.
    //#ifdef ENABLE_OSSL
    ObjectCleanupEvict();
    //#endif

    // The parameters and sessions have been marshaled. Now tack on the header and
    // set the sizes
    //#ifdef ENABLE_OSSL
    // LOG_INFO("RES SIZE: %d", ARRAYSIZE(response));
    // for (int i = 0; i < ARRAYSIZE(response); i++){
    //     LOG_INFO("RES %02x", *response[i]);
    // }
    BuildResponseHeader(&command, *response, result);
    if (result != 0){
        LOG_INFO("DEBUG INFO: BuildResponse Result = %d", result);
    }
    //#endif

    // Try to commit all the writes to NV if any NV write happened during this
    // command execution. This check should be made for both succeeded and failed
    // commands, because a failed one may trigger a NV write in DA logic as well.
    // This is the only place in the command execution path that may call the NV
    // commit. If the NV commit fails, the TPM should be put in failure mode.
    if((g_updateNV != UT_NONE) && !g_inFailureMode)
    {
        LOG_INFO("DEBUG INFO: g_updateNV = %d g_inFailureMode = %d", g_updateNV, g_inFailureMode);
        if(g_updateNV == UT_ORDERLY)
            NvUpdateIndexOrderlyData();
        if(!NvCommit())
            FAIL(FATAL_ERROR_INTERNAL);
        g_updateNV = UT_NONE;
    }
    #ifdef DEBUG_PRINT
    LOG_INFO("DEBUG INFO: Parameter Size = %d MAX = %d", command.parameterSize, maxResponse);
    #endif
    pAssert((UINT32)command.parameterSize <= maxResponse);

    // Clear unused bits in response buffer.
    //#ifdef ENABLE_OSSL
    MemorySet(*response + *responseSize, 0, maxResponse - *responseSize);
    //#endif
    // as a final act, and not before, update the response size.
    *responseSize = (UINT32)command.parameterSize;

    return;
}

bool test_main(void) {

    //gc.shEnable = 1;

  CHECK_DIF_OK(dif_pinmux_init(
      mmio_region_from_addr(TOP_EARLGREY_PINMUX_AON_BASE_ADDR), &pinmux));

  CHECK_DIF_OK(dif_spi_device_init_handle(
      mmio_region_from_addr(TOP_EARLGREY_SPI_DEVICE_BASE_ADDR), &spi_device));

  CHECK_DIF_OK(dif_rv_plic_init(
      mmio_region_from_addr(TOP_EARLGREY_RV_PLIC_BASE_ADDR), &plic));

  //Init the CSRNG and Entropy Sources 
  
  init_CSRNG_peripherals();

  g_DRTMHandle = TPM_RH_UNASSIGNED;
  // Set IoA7 for tpm csb.
  // Longer term this needs to migrate to a top specific, platform specific
  // setting.
  CHECK_DIF_OK(dif_pinmux_input_select(
      &pinmux, kTopEarlgreyPinmuxPeripheralInSpiDeviceTpmCsb,
      kTopEarlgreyPinmuxInselIoa7));

  if (kDeviceType == kDeviceSimDV) {
    dif_pinmux_pad_attr_t out_attr;
    dif_pinmux_pad_attr_t in_attr = {
        .slew_rate = 0,
        .drive_strength = 0,
        .flags = kDifPinmuxPadAttrPullResistorEnable |
                 kDifPinmuxPadAttrPullResistorUp};

    CHECK_DIF_OK(dif_pinmux_pad_write_attrs(&pinmux, kTopEarlgreyMuxedPadsIoa7,
                                            kDifPinmuxPadKindMio, in_attr,
                                            &out_attr));
  }

  ptrdiff_t reg_offset =
    PINMUX_MIO_PERIPH_INSEL_0_REG_OFFSET + (ptrdiff_t)(46 << 2);
  mmio_region_write32(pinmux.base_addr,reg_offset, 1);

  static const uint32_t disable_SPI_mode[1] = {0x00000000};
  mmio_region_memcpy_to_mmio32(spi_device.dev.base_addr, SPI_DEVICE_CONTROL_REG_OFFSET, disable_SPI_mode, 4 );

  static const uint32_t enable_intr[1] = {0x000000E0};
  mmio_region_memcpy_to_mmio32(spi_device.dev.base_addr, SPI_DEVICE_TPM_INT_ENABLE_REG_OFFSET, enable_intr, 4 );

  mmio_region_write32(pinmux.base_addr,reg_offset, 0);

  // Configure fast slew rate and strong drive strength for SPI device pads.
  CHECK_STATUS_OK(spi_device_testutils_configure_pad_attrs(&pinmux));

  CHECK_STATUS_OK(entropy_testutils_auto_mode_init());

  CHECK_DIF_OK(
      dif_spi_device_tpm_configure(&spi_device, kDifToggleEnabled, kTpmConfig));

  //Configure CSRNG 

  CHECK_DIF_OK(dif_spi_device_tpm_set_sts_reg(&spi_device, 0x00000018));

  CHECK_DIF_OK(dif_spi_device_tpm_set_intf_capability_reg(&spi_device, 0x00000014));


  // enable interrupts
  en_plic_irqs();
  en_spi_device_irqs(&spi_device.dev);

  // Sync message with testbench to begin.
  // Wait for write interrupt.
  _TPM_Init();

  gp.lockOutAuthEnabled = TRUE;
  while(1){
    LOG_INFO("SYNC: Begin TPM Test");
  ATOMIC_WAIT_FOR_INTERRUPT(header_interrupt_received);
  
  // Check what command we have received. Store it as expected variables
  // and compare when the read command is issued.
  uint8_t write_command = 0;
  uint32_t write_addr = 0;
  CHECK_DIF_OK(dif_spi_device_tpm_get_command(&spi_device, &write_command,
                                              &write_addr));
  //#ifdef NO_VERILATOR
  //LOG_INFO("Write Command: %02x",write_command);
  //LOG_INFO("Write Address: %08x",write_addr );
 // #endif
  CHECK((write_command & kTpmCommandRwMask) == kTpmWriteCommand,
        "Expected write command, received read");
  // Poll for write data to complete.
  uint32_t num_bytes = (write_command & kTpmCommandSizeMask) + 1;
    //LOG_INFO("Expecting %d bytes from tpm write wr_cmd : %08x", num_bytes, write_command);
    if(num_bytes == 1){
        LOG_INFO("ERROR IN CMD: CMD: %08x ADDR: %08x", write_command, write_addr );
        CHECK_DIF_OK(dif_spi_device_tpm_free_write_fifo(&spi_device));
        ack_spi_tpm_header_irq(&spi_device);
        goto End;
    }
  uint8_t buf[64] = {0};
  uint8_t total_buf[256] = {0};
  dif_result_t status = kDifOutOfRange;
  uint8_t bytes_received = 0;
  #ifdef NO_VERILATOR
  LOG_INFO("status for kDIFOutofRange: %d", status);
  LOG_INFO("Spi->dev.base_addr: %08x", spi_device.dev.base_addr);
  #endif 
  while (status == kDifOutOfRange) {
    //LOG_INFO("Attempt read : %d", mmio_region_read32(spi_device.dev.base_addr,(SPI_DEVICE_INGRESS_BUFFER_REG_OFFSET)));
    //mmio_region_memcpy_from_mmio32(spi_device.dev.base_addr, 0x3E0, buf, num_bytes);
    status = dif_spi_device_tpm_read_data(&spi_device, num_bytes, buf);
    uint32_t size = buf[5];
    memcpy(total_buf, buf, num_bytes);
    size = size - num_bytes;
    //LOG_INFO("TOTAL SIZE: %d NUM_BYTES: %d", size, num_bytes);
    uint8_t read_count = 0;
    bytes_received = num_bytes;
    //Continue to read 
    while( size > 0){
        num_bytes = 0;
        memset(buf, 0x00, 64);
        CHECK_DIF_OK(dif_spi_device_tpm_free_write_fifo(&spi_device));
        ack_spi_tpm_header_irq(&spi_device);

        mmio_region_write32(pinmux.base_addr,reg_offset, 1);
        mmio_region_write32(pinmux.base_addr,reg_offset, 0);
        LOG_INFO("Waiting on TPM WRITE: Count = %d", read_count);
        read_count ++;
        ATOMIC_WAIT_FOR_INTERRUPT(header_interrupt_received);
        CHECK_DIF_OK(dif_spi_device_tpm_get_command(&spi_device, &write_command,
                                            &write_addr));
        uint32_t num_bytes = (write_command & kTpmCommandSizeMask) + 1;
        status = dif_spi_device_tpm_read_data(&spi_device, num_bytes, buf);
        // for (int i = 0; i < num_bytes; i++){
        // LOG_INFO("DEBUG INFO: Read this info from the SPI Buf[%d]: %02x ",i, buf[i]);
        // }       
        memcpy(&total_buf[bytes_received], buf, num_bytes);
        bytes_received = bytes_received + num_bytes;
        size = size - num_bytes;
        //LOG_INFO("SIZE: %d, NUM_BYTES: %d", size, num_bytes);

    }
    //#define DEBUG_PRINT
    #ifdef DEBUG_PRINT
    // for (int i = 0; i < bytes_received; i++){
    //  LOG_INFO("DEBUG INFO: Read this info from the SPI Buf[%d]: %02x ",i, total_buf[i]);
    // }
    for (int i = 0; i < bytes_received; i += 16) {
    char debug_str[128]; // Buffer for the formatted string
    int len = 0;
    
    len += snprintf(debug_str + len, sizeof(debug_str) - len, "DEBUG INFO: Read this info from the SPI Buf[%d]:", i);
    
    for (int j = 0; j < 16 && (i + j) < bytes_received; j++) {
        len += snprintf(debug_str + len, sizeof(debug_str) - len, " %02x", total_buf[i + j]);
    }
    
    LOG_INFO("%s", debug_str);
}
    LOG_INFO("LAST BYTE: %02x No. of Bytes %d", total_buf[bytes_received], bytes_received);
    #endif
  };
  CHECK_DIF_OK(status);

  //Execute the command 

  _OUT_BUFFER response; 
  response.BufferSize = bytes_received;
  char OutputBuffer[1024];
  response.Buffer = (_OUTPUT_BUFFER)OutputBuffer;
  uint32_t responseSize = 1024;
  ExecuteCommand(bytes_received, (unsigned char *)total_buf, &responseSize, &response.Buffer );

    for (int i = 0; i < responseSize; i += 16) {
    char debug_str[512]; // Buffer for the formatted string
    int len = 0;
    
    len += snprintf(debug_str + len, sizeof(debug_str) - len, "DEBUG INFO: RESPONSE[%d]:", i);
    
    for (int j = 0; j < 16 && (i + j) < responseSize; j++) {
        len += snprintf(debug_str + len, sizeof(debug_str) - len, " %02x", response.Buffer[i + j]);
    }
    
    LOG_INFO("%s", debug_str);
    }
  
 // LOG_INFO("EXECUTED COMMAND");
  //LOG_INFO("Response Created = %04x bytes", responseSize);
  ///#ifdef NO_VERILATOR
//   for(int i = 0; i < responseSize; i++){
//     LOG_INFO("DEBUG INFO: BUF[%d] = %02x", i, response.Buffer[i]);
//     }
  //#endif

  // Finished processing the write command
  CHECK_DIF_OK(dif_spi_device_tpm_free_write_fifo(&spi_device));
  ack_spi_tpm_header_irq(&spi_device);
  
  mmio_region_write32(pinmux.base_addr,reg_offset, 1);
  mmio_region_write32(pinmux.base_addr,reg_offset, 0);

#ifdef READ
LOG_INFO("SYNC: Waiting Read");


// // Wait for read interrupt.
ATOMIC_WAIT_FOR_INTERRUPT(header_interrupt_received);

uint8_t read_command = 0;
uint32_t read_addr = 0;
CHECK_DIF_OK(
    dif_spi_device_tpm_get_command(&spi_device, &read_command, &read_addr));

uint32_t read_amt = (read_command & kTpmCommandSizeMask) + 1;
//Check if there will be multiple reads
if(responseSize > 64){
      //If the reponse is greater than 64 bytes, write to the FIFO in 64 byte blocks
  uint32_t index = 0;
  while(responseSize > 16 * sizeof(uint32_t)){
    CHECK_DIF_OK(dif_spi_device_tpm_write_data(&spi_device, 64, &response.Buffer[index]));
    ack_spi_tpm_header_irq(&spi_device);
    index += 64;
    responseSize -= 64;

    for (int i = 0; i < 100000; i ++){
    __asm__ volatile ("" ::: "memory");
    }
    mmio_region_write32(pinmux.base_addr,reg_offset, 1);
    mmio_region_write32(pinmux.base_addr,reg_offset, 0);

    LOG_INFO("Index: %d ResponseSize: %d", index, responseSize);
    ATOMIC_WAIT_FOR_INTERRUPT(header_interrupt_received);
  }

}
else{
    CHECK_DIF_OK(dif_spi_device_tpm_write_data(&spi_device, read_amt, (uint8_t *)response.Buffer));
    ack_spi_tpm_header_irq(&spi_device);

    for (int i = 0; i < 100000; i ++){
    __asm__ volatile ("" ::: "memory");
  }
    mmio_region_write32(pinmux.base_addr,reg_offset, 1);
    mmio_region_write32(pinmux.base_addr,reg_offset, 0);

}
#endif
End:
  mmio_region_write32(pinmux.base_addr,reg_offset, 1);
  mmio_region_write32(pinmux.base_addr,reg_offset, 0);
  }

  return true;
}
