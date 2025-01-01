#!/bin/bash


# This script produces a diff between two versions of 'tasks.c':
# (i) The production version of the source file and (ii) the verified version.
# The diff is computed from the preprocessed version of both files which include
# all code relevant to the proof. That is, that any change in a file required
# by the VeriFast proof will shot up in the diff.
# The diff report will be written to 'stats/diff_report.txt' directory.
#
# This script expects the following arguments:
# $1 : Absolute path to the base directory of this repository.


# Checking validity of command line arguments.
HELP="false"
if [ $1 == "-h" ] || [ $1 == "--help" ]; then
    HELP="true"
else
    if [ $# != 1 ] ; then
        echo Wrong number of arguments. Found $#, expected 1.
        HELP="true"
    fi

    if [ ! -d "$1" ]; then
        echo Directory "$1" does not exist.
        HELP="true"
    fi
fi

if [ "$HELP" != "false" ]; then
    echo Expected call of the form
    echo "diff.sh <REPO_BASE_DIR>"
    echo "where <REPO_BASE_DIR> is the absolute path to the base directory of this repository."
    exit
fi


# Relative or absolute path to the directory this script and `paths.sh` reside in.
PREFIX=`dirname $0`
# Absolute path to the base of this repository.
REPO_BASE_DIR="$1"


# Load functions used to compute paths.
. "$PREFIX/paths.sh"


VF_PROOF_BASE_DIR=`vf_proof_base_dir $REPO_BASE_DIR`
PP_SCRIPT_DIR=`pp_script_dir $REPO_BASE_DIR`
PP="$PP_SCRIPT_DIR/preprocess_file_for_diff.sh"
LOG_DIR=`pp_log_dir $REPO_BASE_DIR`
STATS_DIR=`stats_dir $REPO_BASE_DIR`

# Unpreprocessed verions of tasks.c
PROD_TASKS_C=`prod_tasks_c $REPO_BASE_DIR`
VF_TASKS_C=`vf_annotated_tasks_c $REPO_BASE_DIR`

# Preprocessed versions of tasks.c
PP_OUT_DIR=`pp_out_dir $REPO_BASE_DIR`
PP_PROD_TASKS_C=`pp_prod_tasks_c $REPO_BASE_DIR`
PP_VF_TASKS_C=`pp_vf_tasks_c $REPO_BASE_DIR`

ensure_output_dirs_exist $REPO_BASE_DIR

echo preprocessing production version of 'tasks.c'
$PP $PROD_TASKS_C $PP_PROD_TASKS_C \
    "$LOG_DIR/pp_prod_tasks_c_error_report.txt" \
    $REPO_BASE_DIR $VF_PROOF_BASE_DIR

echo preprocessing verified version of 'tasks.c'
$PP $VF_TASKS_C $PP_VF_TASKS_C \
    "$LOG_DIR/pp_vf_tasks_c_error_report.txt" \
    $REPO_BASE_DIR $VF_PROOF_BASE_DIR


echo Computing diff:
echo

git diff --no-index --ignore-all-space $PP_PROD_TASKS_C $PP_VF_TASKS_C
