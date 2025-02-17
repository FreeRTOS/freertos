#!/bin/bash


# This script preprocesses a given source file annotated with VeriFast proof
# steps. Include paths are configured to fit 'tasks.c', but it might also be
# useful for other source files. The preprocessor is configured to include the
# proper proof files from VeriFast's standard library and to also include
# source code guarded by 'VERIFAST' defines.
#
# This script expects the following arguments:
# $1 : Absolute path to the source file to be preprocessed.
# $2 : Absolute path of the preprocessor's output file.
# $3 : Absolute path to which the error report will be written.
# $4 : Absolute path to the base directory of this repository.
# $5 : Absolute path to the VeriFast proof directory.
# $6 : Absolute path to the VeriFast installation directory.


SRC_FILE="$1"
OUT_FILE="$2"
ERR_FILE="$3"
REPO_BASE_DIR="$4"
VF_PROOF_BASE_DIR="$5"
VF_DIR="$6"



# Load functions used to compute paths.
. "$VF_PROOF_BASE_DIR/paths.sh"

# Load variables storing preprocessor flags.
. "`pp_script_dir $REPO_BASE_DIR`/pp_flags.sh" "$REPO_BASE_DIR" "$VF_PROOF_BASE_DIR" "$VF_DIR"


# Relevant clang flags:
# -E : Run preprocessor
# -C : Include comments in output
# -P : Surpresses line/file pragmas

echo start preprocessor
clang -E -C \
\
${BUILD_FLAGS[@]} \
${VERIFAST_FLAGS[@]} \
${RP2040_INLCUDE_FLAGS[@]} \
${PICO_INCLUDE_FLAGS[@]} \
-I`prod_header_dir $REPO_BASE_DIR` \
\
-c "$SRC_FILE" \
1>"$OUT_FILE" 2>"$ERR_FILE"
