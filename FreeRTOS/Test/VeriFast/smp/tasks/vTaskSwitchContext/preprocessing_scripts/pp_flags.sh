#!/bin/bash

# This script defines common command line arguments for the preprocessor.

# This script expects the following arguments:
# $1 : Absolute path to the base directory of this repository.
# $2 : Absolute path to the VeriFast proof directory.
# $3 : Absolute path to the VeriFast installation directory.


REPO_BASE_DIR="$1"
VF_PROOF_BASE_DIR="$2"
VF_DIR="$3"


# Load functions used to compute paths.
. "$VF_PROOF_BASE_DIR/paths.sh"

PICO_SDK_DIR=`pico_sdk_dir $REPO_BASE_DIR`
SMP_DEMO_DIR=`smp_demo_dir $REPO_BASE_DIR`
VF_PROOF_MOD_HEADER_DIR=`vf_proof_mod_header_dir $REPO_BASE_DIR`
VF_PROOF_MOD_SRC_DIR=`vf_proof_mod_src_dir $REPO_BASE_DIR`
PROOF_SETUP_DIR=`vf_proof_setup_dir $REPO_BASE_DIR`
PROOF_FILES_DIR=`vf_proof_dir $REPO_BASE_DIR`



declare -a BUILD_FLAGS
BUILD_FLAGS=(
    -DFREE_RTOS_KERNEL_SMP=1
    -DLIB_FREERTOS_KERNEL=1
    -DLIB_PICO_BIT_OPS=1
    -DLIB_PICO_BIT_OPS_PICO=1
    -DLIB_PICO_DIVIDER=1
    -DLIB_PICO_DIVIDER_HARDWARE=1
    -DLIB_PICO_DOUBLE=1
    -DLIB_PICO_DOUBLE_PICO=1
    -DLIB_PICO_FLOAT=1
    -DLIB_PICO_FLOAT_PICO=1
    -DLIB_PICO_INT64_OPS=1
    -DLIB_PICO_INT64_OPS_PICO=1
    -DLIB_PICO_MALLOC=1
    -DLIB_PICO_MEM_OPS=1
    -DLIB_PICO_MEM_OPS_PICO=1
    -DLIB_PICO_MULTICORE=1
    -DLIB_PICO_PLATFORM=1
    -DLIB_PICO_PRINTF=1
    -DLIB_PICO_PRINTF_PICO=1
    -DLIB_PICO_RUNTIME=1
    -DLIB_PICO_STANDARD_LINK=1
    -DLIB_PICO_STDIO=1
    -DLIB_PICO_STDIO_UART=1
    -DLIB_PICO_STDLIB=1
    -DLIB_PICO_SYNC=1
    -DLIB_PICO_SYNC_CORE=1
    -DLIB_PICO_SYNC_CRITICAL_SECTION=1
    -DLIB_PICO_SYNC_MUTEX=1
    -DLIB_PICO_SYNC_SEM=1
    -DLIB_PICO_TIME=1
    -DLIB_PICO_UTIL=1
    -DPICO_BOARD=\"pico\"
    -DPICO_BUILD=1
    -DPICO_CMAKE_BUILD_TYPE=\"Release\"
    -DPICO_COPY_TO_RAM=0
    -DPICO_CXX_ENABLE_EXCEPTIONS=0
    -DPICO_NO_FLASH=0
    -DPICO_NO_HARDWARE=0
    -DPICO_ON_DEVICE=1
    -DPICO_STACK_SIZE=0x1000
    -DPICO_TARGET_NAME=\"on_core_one\"
    -DPICO_USE_BLOCKED_RAM=0
    -DmainRUN_FREE_RTOS_ON_CORE=1
)

declare -a PICO_INCLUDE_FLAGS
PICO_INCLUDE_FLAGS=(
    -I"pico-sdk"
)

declare -a RP2040_INCLUDE_FLAGS
RP2040_INLCUDE_FLAGS=(
    -I"$VF_PROOF_BASE_DIR/portable/ThirdParty/GCC/RP2040/include"
    -I"$VF_PROOF_BASE_DIR/FreeRTOS/Source/portable/ThirdParty/GCC/RP2040"
)

declare -a VERIFAST_FLAGS
VERIFAST_FLAGS=(
    -DVERIFAST
    -DVERIFAST_SKIP_BITVECTOR_PROOF__STACK_ALIGNMENT
    -I"$VF_DIR/bin"
    -I"$VF_PROOF_MOD_HEADER_DIR"
    -I"$VF_PROOF_MOD_SRC_DIR"
    -I"$PROOF_SETUP_DIR"
    -I"$PROOF_FILES_DIR"
)
