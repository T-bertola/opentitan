
load("@bazel_skylib//lib:dicts.bzl", "dicts")
load(
    "//rules:const.bzl",
    "CONST",
    "get_lc_items",
)
load(
    "//rules:otp.bzl",
    "STD_OTP_OVERLAYS",
    "otp_image",
    "otp_json",
    "otp_partition",
)
load(
    "//rules/opentitan:defs.bzl",
    "EARLGREY_CW340_TEST_ENVS",
    "EARLGREY_SILICON_OWNER_ROM_EXT_ENVS",
    "EARLGREY_TEST_ENVS",
    "OPENTITAN_CPU",
    "dv_params",
    "ecdsa_key_for_lc_state",
    "fpga_params",
    "opentitan_binary",
    "opentitan_test",
    "silicon_params",
    "verilator_params",
)
load("//rules/opentitan:keyutils.bzl", "ECDSA_ONLY_KEY_STRUCTS")

package(default_visibility = ["//visibility:public"])



cc_library(
    name = "TPM_LIB",
    srcs = glob([
        "TPMCmd/tpm/cryptolibs/common/**/*.c",
        "TPMCmd/tpm/cryptolibs/Ossl/**/*.c",
        "TPMCmd/tpm/cryptolibs/TpmBigNum/**/*.c",
        "TPMCmd/tpm/src/**/*.c",
        "TPMCmd/tpm/**/*.inl",
        "TPMCmd/Platform/**/*.c"
        
    ]),
    hdrs = glob([
        "TPMCmd/tpm/cryptolibs/common/**/*.h",
        "TPMCmd/tpm/cryptolibs/Ossl/**/*.h",
        "TPMCmd/tpm/cryptolibs/TpmBigNum/**/*.h",
        "TPMCmd/tpm/include/**/*.h",
        "TPMCmd/TpmConfiguration/**/*.h",
        "TPMCmd/Platform/**/*.h",
        "TPMCmd/tpm/src/**/*.h"
        
        
    ]),
    linkopts = [
        "-L/usr/include/openssl-riscv/lib -lssl -lcrypto",
        "-L/home/tcb2196/OpenTitan/ot_tools/riscv/riscv32-unknown-elf/lib -lc",
        "-L/home/tcb2196/OpenTitan/ot_tools/riscv/riscv32-unknown-elf/lib",  # Add the library path
        "-lnosys"
        

    ],
    copts =[
        "-Wno-error=deprecated-declarations",
        "-Wno-sign-conversion",
        "-Wno-implicit-int-conversion",
        "-Wno-implicit-fallthrough",
        "-Wno-error=unknown-attributes",
        "-Wno-error=unused-variable",
        "-Wno-error=uninitialized",
        "-Wno-strict-prototypes",
        "-Wno-zero-length-array",
        "-Wno-error=gnu-empty-initializer",
        "-Wno-error=xor-used-as-pow"
        
    ],
    target_compatible_with = [OPENTITAN_CPU],
    deps = [
        "@openssl_headers//:openssl",
        "//hw/top_earlgrey/sw/autogen:top_earlgrey",
        "//sw/device/lib/arch:device",
        "//sw/device/lib/base:mmio",
        "//sw/device/lib/dif:pinmux",
        "//sw/device/lib/dif:rv_plic",
        "//sw/device/lib/dif:spi_device",
        "//sw/device/lib/dif:csrng",
        "//sw/device/lib/testing:csrng_testutils",
        "//sw/device/lib/runtime:hart",
        "//sw/device/lib/runtime:irq",
        "//sw/device/lib/runtime:log",
        "//sw/device/lib/testing:isr_testutils",
        "//sw/device/lib/testing:spi_device_testutils",
        "//sw/device/lib/testing/test_framework:ottf_main",
        "//sw/device/lib/testing/test_framework:status",
        "//sw/device/lib/dif:edn",
        "//sw/device/lib/dif:entropy_src",
        "//sw/device/lib/testing:entropy_testutils",
        "//sw/device/lib/testing:rand_testutils",
        "//sw/device/lib/crypto/impl:hash",
        "//sw/device/lib/crypto/impl:rsa",
        "//sw/device/lib/crypto/impl/rsa:rsa_datatypes"
    ],
)