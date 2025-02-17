# Returns the absolute path to the directory containing the VeriFast proofs
# concerning `vTaskSwitchContext` in `tasks.c`.
#
# Expected arguments:
# $1 : Absolute path to the repository's base directory.
function vf_proof_base_dir() {
    REPO_BASE_DIR="$1"
    echo "$REPO_BASE_DIR/FreeRTOS/Test/VeriFast/smp/tasks/vTaskSwitchContext"
}

# Returns the absolute path to the directory containing modified versions of
# FreeRTOS source files. The VeriFast proofs use these modified verions instead
# of the original files.
#
# Expected arguments:
# $1 : Absolute path to the repository's base
function vf_proof_mod_src_dir() {
    REPO_BASE_DIR="$1"
    VF_PROOF_DIR=`vf_proof_base_dir $REPO_BASE_DIR`

    echo "$VF_PROOF_DIR/src"
}

# Returns the absolute path to the directory containing modified versions of
# FreeRTOS header files. The VeriFast proofs use these modified verions instead
# of the original files.
#
# Expected arguments:
# $1 : Absolute path to the repository's base
function vf_proof_mod_header_dir() {
    REPO_BASE_DIR="$1"
    VF_PROOF_DIR=`vf_proof_base_dir $REPO_BASE_DIR`

    echo "$VF_PROOF_DIR/include"
}

# Returns the absolute path to the directory containing everything related to
# the setup of the VeriFast proofs.
#
# Expected arguments:
# $1 : Absolute path to the repository's base
function vf_proof_setup_dir() {
    REPO_BASE_DIR="$1"
    VF_PROOF_DIR=`vf_proof_base_dir $REPO_BASE_DIR`

    echo "$VF_PROOF_DIR/proof_setup"
}

# Returns the absolute path to the directory containing all lemmas and
# definitions used written for the VeriFast proofs.
#
# Expected arguments:
# $1 : Absolute path to the repository's base
function vf_proof_dir() {
    REPO_BASE_DIR="$1"
    VF_PROOF_DIR=`vf_proof_base_dir $REPO_BASE_DIR`

    echo "$VF_PROOF_DIR/proof"
}

# Returns the absolute path to the version of `tasks.c` containing the VeriFast
# proof annotations.
#
# Expected arguments:
# $1 : Absolute path to the repository's base
function vf_annotated_tasks_c() {
    REPO_BASE_DIR="$1"
    VF_MOD_SRC_DIR=`vf_proof_mod_src_dir $REPO_BASE_DIR`

    echo "$VF_MOD_SRC_DIR/tasks.c"
}

# Returns the absolute path to the directory the unmodified FreeRTOS headers.
#
# Expected arguments:
# $1 : Absolute path to the repository's base
function prod_header_dir() {
    REPO_BASE_DIR="$1"

    echo "$REPO_BASE_DIR/FreeRTOS/Source/include"
}

# Returns the absolute path to the directory the unmodified FreeRTOS source
# files.
#
# Expected arguments:
# $1 : Absolute path to the repository's base
function prod_src_dir() {
    REPO_BASE_DIR="$1"

    echo "$REPO_BASE_DIR/FreeRTOS/Source/"
}

# Returns the absolute path to the unmodified version of `tasks.c`.
#
# Expected arguments:
# $1 : Absolute path to the repository's base
function prod_tasks_c() {
    REPO_BASE_DIR="$1"
    PROD_SRC_DIR=`prod_src_dir $REPO_BASE_DIR`

    echo "$PROD_SRC_DIR/tasks.c"
}


# Returns the absolute path to the directory containing the preprocessing scripts.
#
# Expected arguments:
# $1 : Absolute path to the repository's base
function pp_script_dir() {
    REPO_BASE_DIR="$1"
    VF_PROOF_DIR=`vf_proof_base_dir $REPO_BASE_DIR`

    echo "$VF_PROOF_DIR/preprocessing_scripts"
}

# Returns the absolute path to the preprocesor's output direcotry.
#
# Expected arguments:
# $1 : Absolute path to the repository's base
function pp_out_dir() {
    REPO_BASE_DIR="$1"
    VF_PROOF_DIR=`vf_proof_base_dir $REPO_BASE_DIR`

    echo "$VF_PROOF_DIR/preprocessed_files"
}

# Returns the absolute path to the preprocesor's log direcotry.
#
# Expected arguments:
# $1 : Absolute path to the repository's base
function pp_log_dir() {
    REPO_BASE_DIR="$1"
    VF_PROOF_DIR=`vf_proof_base_dir $REPO_BASE_DIR`

    echo "$VF_PROOF_DIR/pp_log"
}

# Returns the absolute path to the preprocessed version of `tasks.c` containing
# the VeriFast proof annotations. This is the file that is processed by
# VeriFast.
#
# Expected arguments:
# $1 : Absolute path to the repository's base
function pp_vf_tasks_c() {
    REPO_BASE_DIR="$1"
    PP_OUT_DIR=`pp_out_dir $REPO_BASE_DIR`

    echo "$PP_OUT_DIR/tasks_vf_pp.c"
}

# Returns the absolute path to the preprocessed unmodified version of `tasks.c`.
#
# Expected arguments:
# $1 : Absolute path to the repository's base
function pp_prod_tasks_c() {
    REPO_BASE_DIR="$1"
    PP_OUT_DIR=`pp_out_dir $REPO_BASE_DIR`

    echo "$PP_OUT_DIR/tasks_prod_pp.c"
}

# Returns the absolute path to the pico sdk.
#
# Expected arguments:
# $1 : Absolute path to the repository's base
function pico_sdk_dir() {
    REPO_BASE_DIR="$1"
    VF_PROOF_DIR=`vf_proof_base_dir $REPO_BASE_DIR`

    echo "$VF_PROOF_DIR/sdks/pico-sdk"
}

# Returns the absolute path to the smp_demo_dir.
#
# Expected arguments:
# $1 : Absolute path to the repository's base
function smp_demo_dir() {
    REPO_BASE_DIR="$1"
    VF_PROOF_DIR=`vf_proof_base_dir $REPO_BASE_DIR`

    echo "$VF_PROOF_DIR/demos/FreeRTOS-SMP-Demos"
}


# Returns the absolute path to directory where the statistic reports are stored.
#
# Expected arguments:
# $1 : Absolute path to the repository's base
function stats_dir() {
    REPO_BASE_DIR="$1"
    VF_PROOF_DIR=`vf_proof_base_dir $REPO_BASE_DIR`

    echo "$VF_PROOF_DIR/stats"
}


# Ensures that all potentially relevant output direcories exist.
#
# Expected arguments:
# $1 : Absolute path to the repository's base
function ensure_output_dirs_exist() {
    REPO_BASE_DIR="$1"

    PP_OUT_DIR=`pp_out_dir $REPO_BASE_DIR`
    STATS_DIR=`stats_dir $REPO_BASE_DIR`
    PP_LOG_DIR=`pp_log_dir $REPO_BASE_DIR`

    if [ ! -d "$PP_OUT_DIR" ]; then
        mkdir "$PP_OUT_DIR"
    fi
    if [ ! -d "$STATS_DIR" ]; then
        mkdir "$STATS_DIR"
    fi
    if [ ! -d "$PP_LOG_DIR" ]; then
        mkdir "$PP_LOG_DIR"
    fi
}
