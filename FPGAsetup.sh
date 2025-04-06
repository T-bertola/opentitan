#!/bin/bash

BOARD=cw340

source /bin/Xilinx/Vivado/2020.2/settings64.sh

echo "Loading Bitstream onto FPGA"

./bazelisk.sh run //sw/host/opentitantool -- --rcfile= --interface=$BOARD fpga set-pll

./bazelisk.sh run //sw/host/opentitantool -- fpga load-bitstream /tmp/bitstream-latest/chip_earlgrey_cw340/lowrisc_systems_chip_earlgrey_$BOARD\_0.1.bit

