#!/bin/bash


# This script preprocesses a given source file. Include paths are configured to
# fit 'tasks.c', but it might also be useful for other source files. 
# The preprocessor is configured such that `diff`-ing results produced by this
# script (from different versions of the same file) yields useful results.
#
# This script expects the following arguments:
# $1 : Absolute path to the source file to be preprocessed.
# $2 : Absolute path of the preprocessor's output file.
# $3 : Absolute path to which the error report will be written.
# $4 : Absolute path to the base directory of this repository.
# $5 : Absolute path to the VeriFast proof directory.


SRC_FILE="$1"
OUT_FILE="$2"
ERR_FILE="$3"
REPO_BASE_DIR="$4"
VF_PROOF_BASE_DIR="$5"


# Load functions used to compute paths.
. "$VF_PROOF_BASE_DIR/paths.sh"

# Load variables storing preprocessor flags.
. "`pp_script_dir $REPO_BASE_DIR`/pp_flags.sh" "$REPO_BASE_DIR" "$VF_PROOF_BASE_DIR"

PROD_HEADER_DIR=`prod_header_dir $REPO_BASE_DIR`


# Relevant clang flags:
# -E : Run preprocessor
# -C : Include comments in output
# -P : Surpresses line/file pragmas
# -D NDEBUG : Deactivate assertions.

# Note: 
# The implementation of the `assert` macro is platform dependent and is defined
# in the system header `assert.h`. A preprocessed assertion might contain
# a reference to the location of the assertion in the source code (e.g. on OS X). 
# This causes false positives when `diff`-ing preprocessed files. Hence, we
# deactivate assertions.

echo Preprocessing file:
echo \"$SRC_FILE\"
echo Output will be written to:
echo \"$OUT_FILE\"
echo Errors will be reported in:
echo \"$ERR_FILE\"
echo
clang -E -P -D NDEBUG \
${BUILD_FLAGS[@]} ${RP2040_INLCUDE_FLAGS[@]} ${PICO_INCLUDE_FLAGS[@]} \
-I"$PROD_HEADER_DIR" \
-c "$SRC_FILE" \
1>"$OUT_FILE" 2>"$ERR_FILE"