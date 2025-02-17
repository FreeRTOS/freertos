#!/bin/bash

# This script expects the following command line arguments:
# $1 : Absolute path to the source file that should be prepared for VeriFast.
# $2 : Absolute path to which the result shall be written.
# $3 : Absolute path under which preprocessor error shall be logged.
# $4 : Absolute path to the root dir of this repository
# $5 : Absolute path to the root of the directory containing the VeriFast proofs
# $6 : Absolute path to the VeriFast directory

SRC_FILE="$1"
OUT_FILE="$2"
FILE_PP_ERR_LOG="$3"
REPO_BASE_DIR="$4"
VF_PROOF_BASE_DIR="$5"
VF_DIR="$6"


# Load functions used to compute paths.
. "$VF_PROOF_BASE_DIR/paths.sh"


PP_SCRIPT_DIR=`pp_script_dir $REPO_BASE_DIR`
PP_LOG_DIR=`pp_log_dir $REPO_BASE_DIR`
FILE_PP_LOG="$PP_LOG_DIR/pp.c"
FILE_RW_LOG="$PP_LOG_DIR/rw.c"


# Ensure that log directory exists
if [ ! -d "$PP_LOG_DIR" ]; then
    mkdir "$PP_LOG_DIR"
fi


# Preprocessing the source file
# Output is written to '$FILE_PP_LOG' and error report is written to
# '$FILE_PP_ERR_LOG'.
"$PP_SCRIPT_DIR/preprocess_file_for_verification.sh" $SRC_FILE \
    $FILE_PP_LOG $FILE_PP_ERR_LOG \
    $REPO_BASE_DIR $VF_PROOF_BASE_DIR $VF_DIR

cp "$FILE_PP_LOG" "$FILE_RW_LOG"
"$PP_SCRIPT_DIR/vf_rewrite.sh" "$FILE_RW_LOG"
cp "$FILE_RW_LOG" "$OUT_FILE"
