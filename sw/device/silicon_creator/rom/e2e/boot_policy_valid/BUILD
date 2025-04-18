# Copyright lowRISC contributors (OpenTitan project).
# Licensed under the Apache License, Version 2.0, see LICENSE for details.
# SPDX-License-Identifier: Apache-2.0

load(
    "//rules/opentitan:defs.bzl",
    "DEFAULT_TEST_FAILURE_MSG",
    "cw310_params",
    "fpga_params",
    "opentitan_test",
)
load(
    "//rules:const.bzl",
    "CONST",
    "get_lc_items",
    "hex_digits",
)
load(
    "//rules:otp.bzl",
    "STD_OTP_OVERLAYS",
    "otp_image",
)
load(
    "//rules:rom_e2e.bzl",
    "maybe_skip_in_ci",
)
load(
    "//rules:splice.bzl",
    "bitstream_splice",
)
load(
    "//sw/device/silicon_creator/rom/e2e:defs.bzl",
    "MSG_PASS",
    "MSG_TEMPLATE_BFV",
    "SLOTS",
)

package(default_visibility = ["//visibility:public"])

BOOT_POLICY_VALID_CASES = [
    {
        "desc": "good",
        "suffix": "",
    },
    {
        "desc": "bad",
        "suffix": "_corrupted_fpga_cw310_rom_with_fake_keys_signed_bin",
    },
]

[
    otp_image(
        name = "otp_img_boot_policy_valid_{}".format(lc_state),
        src = "//hw/ip/otp_ctrl/data:otp_json_{}".format(lc_state),
        overlays = STD_OTP_OVERLAYS,
    )
    for lc_state, _ in get_lc_items()
]

# Splice OTP images into bitstreams
[
    bitstream_splice(
        name = "bitstream_boot_policy_valid_{}".format(lc_state),
        src = "//hw/bitstream:mask_rom",
        data = ":otp_img_boot_policy_valid_{}".format(lc_state),
        instance = "otp",
        meminfo = "//hw/bitstream:cw310_mmi",
        update_usr_access = True,
    )
    for lc_state, _ in get_lc_items()
]

[
    opentitan_test(
        name = "boot_policy_valid_{}_a_{}_b_{}".format(
            lc_state,
            a["desc"],
            b["desc"],
        ),
        exec_env = {
            "//hw/top_earlgrey:fpga_cw310_rom_with_fake_keys": None,
        },
        fpga = fpga_params(
            assemble = "{{slot_a}}@{} {{slot_b}}@{}".format(
                SLOTS["a"],
                SLOTS["b"],
            ),
            binaries = {
                # PROD keys can sign binaries able to run across all life cycle states.
                "//sw/device/silicon_creator/rom/e2e:empty_test_slot_a_fake_ecdsa_prod_key_0{}".format(a["suffix"]): "slot_a",
                "//sw/device/silicon_creator/rom/e2e:empty_test_slot_b_fake_ecdsa_prod_key_0{}".format(b["suffix"]): "slot_b",
            },
            bitstream = "bitstream_boot_policy_valid_{}".format(lc_state),
            exit_failure = MSG_PASS if a["desc"] == b["desc"] and a["desc"] == "bad" else DEFAULT_TEST_FAILURE_MSG,
            exit_success = MSG_TEMPLATE_BFV.format(hex_digits(CONST.BFV.SIGVERIFY.BAD_ECDSA_SIGNATURE)) if a["desc"] == b["desc"] and a["desc"] == "bad" else MSG_PASS,
            tags = maybe_skip_in_ci(lc_state_val),
        ),
    )
    for lc_state, lc_state_val in get_lc_items()
    for a in BOOT_POLICY_VALID_CASES
    for b in BOOT_POLICY_VALID_CASES
]

test_suite(
    name = "rom_e2e_boot_policy_valid",
    tags = ["manual"],
    tests = [
        "boot_policy_valid_{}_a_{}_b_{}".format(
            lc_state,
            a["desc"],
            b["desc"],
        )
        for lc_state, _ in get_lc_items()
        for a in BOOT_POLICY_VALID_CASES
        for b in BOOT_POLICY_VALID_CASES
    ],
)
