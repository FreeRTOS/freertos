#!/usr/bin/env python3
#
# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License"). You may not
# use this file except in compliance with the License. A copy of the License is
# located at
#
#     http://aws.amazon.com/apache2.0/
#
# or in the "license" file accompanying this file. This file is distributed on
# an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
# or implied. See the License for the specific language governing permissions
# and limitations under the License.


import argparse
import logging
import math
import os
import pathlib
import subprocess
import sys


DESCRIPTION = "Configure and run all CBMC proofs in parallel"
# Keep this hard-wrapped at 70 characters, as it gets printed verbatim
# in the terminal. 70 characters stops here -----------------------> |
EPILOG = """
This tool automates the process of running `make report` in each of
the CBMC proof directories. The tool calculates the dependency graph
of all tasks needed to build, run, and report on all the proofs, and
executes these tasks in parallel.

The tool is roughly equivalent to doing this:

        litani init --project "FreeRTOS";

        for proof in $(find . -name cbmc-batch.yaml); do
            pushd $(dirname ${proof});
            make report;
            popd;
        done

        litani run-build;

except that it is much faster and provides some convenience options.
The CBMC CI runs this script with no arguments to build and run all
proofs in parallel.

The --no-standalone argument omits the `litani init` and `litani
run-build`; use it when you want to add additional proof jobs, not
just the CBMC ones. In that case, you would run `litani init`
yourself; then run `run-cbmc-proofs --no-standalone`; add any
additional jobs that you want to execute with `litani add-job`; and
finally run `litani run-build`.
"""


def get_args():
    pars = argparse.ArgumentParser(
        description=DESCRIPTION, epilog=EPILOG,
        formatter_class=argparse.RawDescriptionHelpFormatter)
    for arg in [{
            "flags": ["-j", "--parallel-jobs"],
            "type": int,
            "metavar": "N",
            "help": "run at most N proof jobs in parallel",
    }, {
            "flags": ["--no-standalone"],
            "action": "store_true",
            "help": "only configure proofs: do not initialize nor run",
    }, {
            "flags": ["-p", "--proofs"],
            "nargs": "+",
            "metavar": "DIR",
            "help": "only run proof in directory DIR (can pass more than one)",
    }, {
            "flags": ["--project-name"],
            "metavar": "NAME",
            "default": "FreeRTOS",
            "help": "Project name for report. Default: %(default)s",
    }, {
            "flags": ["--verbose"],
            "action": "store_true",
            "help": "verbose output",
    }]:
        flags = arg.pop("flags")
        pars.add_argument(*flags, **arg)
    return pars.parse_args()


def set_up_logging(verbose):
    if verbose:
        level = logging.DEBUG
    else:
        level = logging.WARNING
    logging.basicConfig(
        format="run-cbmc-proofs: %(message)s", level=level)


def print_counter(counter):
    print(
        "\rConfiguring CBMC proofs: "
        "{complete:{width}} / {total:{width}}".format(
            **counter), end="", file=sys.stderr)


def get_proof_dirs(proof_root, proof_list):
    if proof_list is not None:
        proofs_remaining = list(proof_list)
    else:
        proofs_remaining = []

    for root, _, fyles in os.walk(proof_root):
        proof_name = str(pathlib.Path(root).name)
        if proof_list and proof_name not in proof_list:
            continue
        if proof_list and proof_name in proofs_remaining:
            proofs_remaining.remove(proof_name)
        if "cbmc-batch.yaml" in fyles:
            yield pathlib.Path(root)

    if proofs_remaining:
        logging.error(
            "The following proofs were not found: %s",
            ", ".join(proofs_remaining))
        sys.exit(1)


def run_cmd(cmd, **args):
    if "shell" in args and args["shell"]:
        if not isinstance(cmd, str):
            raise UserWarning("Command must be a string if shell=True")
        str_cmd = cmd
    else:
        if not isinstance(cmd, list):
            raise UserWarning("Command passed to run_cmd must be a list")
        str_cmd = " ".join(cmd)

    logging.info("Command: %s", str_cmd)

    proc = subprocess.run(cmd, **args)

    if proc.returncode:
        logging.error("Command failed: %s", str_cmd)

    return proc


def run_build(jobs):
    cmd = ["litani", "run-build"]
    if jobs:
        cmd.extend(["-j", str(jobs)])
    run_cmd(cmd, check=True)


def add_proof_jobs(proof_directory, proof_root):
    proof_directory = pathlib.Path(proof_directory)
    harnesses = [
        f for f in os.listdir(proof_directory) if f.endswith("_harness.c")]
    if not len(harnesses) == 1:
        logging.error(
            "Found %d harnesses in directory '%s'", len(harnesses),
            proof_directory)
        return False

    # Neither the harness name nor the proof directory is unique enough,
    # due to proof configurations. Use the relative path instead.
    proof_name = str(proof_directory.relative_to(proof_root))

    goto_binary = str(
        (proof_directory / ("%s.goto" % harnesses[0][:-2])).resolve())

    # Build goto-binary

    run_cmd([
        "litani", "add-job",
        "--command", "make -B veryclean goto",
        "--outputs", goto_binary,
        "--pipeline-name", proof_name,
        "--ci-stage", "build",
        "--cwd", str(proof_directory),
        "--description", ("%s: building goto-binary" % proof_name),
    ], check=True)

    # Run 3 CBMC tasks

    cbmc_out = str(proof_directory / "cbmc.xml")
    run_cmd([
        "litani", "add-job",
        "--command", "make cbmc",
        "--inputs", goto_binary,
        "--outputs", cbmc_out,
        "--pipeline-name", proof_name,
        "--ci-stage", "test",
        "--tags", "stats-group:safety check",
        # Make returns 2 if the underlying command exited abnormally
        "--ignore-returns", "2",
        "--cwd", str(proof_directory),
        "--description", ("%s: running safety checks" % proof_name),
    ], check=True)

    property_out = str(proof_directory / "property.xml")
    run_cmd([
        "litani", "add-job",
        "--command", "make property",
        "--inputs", goto_binary,
        "--outputs", property_out,
        "--pipeline-name", proof_name,
        "--ci-stage", "test",
        "--cwd", str(proof_directory),
        "--description", ("%s: printing properties" % proof_name),
    ], check=True)

    coverage_out = str(proof_directory / "coverage.xml")
    run_cmd([
        "litani", "add-job",
        "--command", "make coverage",
        "--inputs", goto_binary,
        "--outputs", coverage_out,
        "--pipeline-name", proof_name,
        "--ci-stage", "test",
        "--cwd", str(proof_directory),
        "--tags", "stats-group:coverage computation",
        "--description", ("%s: computing coverage" % proof_name),
    ], check=True)

    # Check whether the CBMC proof actually passed. More details in the
    # Makefile rule for check-cbmc-result.
    run_cmd([
        "litani", "add-job",
        "--command", "make check-cbmc-result",
        "--inputs", cbmc_out,
        "--pipeline-name", proof_name,
        "--ci-stage", "report",
        "--cwd", str(proof_directory),
        "--description", ("%s: checking CBMC result" % proof_name),
    ], check=True)

    # Generate report
    run_cmd([
        "litani", "add-job",
        "--command", "make report",
        "--inputs", cbmc_out, property_out, coverage_out,
        "--outputs", str(proof_directory / "html"),
        "--pipeline-name", proof_name,
        "--ci-stage", "report",
        "--cwd", str(proof_directory),
        "--tags", "stats-group:report generation",
        "--description", ("%s: generating report" % proof_name),
    ], check=True)

    return True


def configure_proof_dirs(proof_dirs, proof_root, counter):
    for proof_dir in proof_dirs:
        print_counter(counter)

        success = add_proof_jobs(proof_dir, proof_root)

        counter["pass" if success else "fail"].append(proof_dir)
        counter["complete"] += 1


def main():
    args = get_args()
    set_up_logging(args.verbose)

    proof_root = pathlib.Path(__file__).resolve().parent

    run_cmd(["./prepare.py"], check=True, cwd=str(proof_root))
    if not args.no_standalone:
        run_cmd(
            ["litani", "init", "--project", args.project_name], check=True)

    proof_dirs = list(get_proof_dirs(proof_root, args.proofs))
    if not proof_dirs:
        logging.error("No proof directories found")
        sys.exit(1)

    counter = {
        "pass": [],
        "fail": [],
        "complete": 0,
        "total": len(proof_dirs),
        "width": int(math.log10(len(proof_dirs))) + 1
    }

    configure_proof_dirs(proof_dirs, proof_root, counter)

    print_counter(counter)
    print("", file=sys.stderr)

    if counter["fail"]:
        logging.error(
            "Failed to configure the following proofs:\n%s", "\n".join(
                [str(f) for f in counter["fail"]]))

    if not args.no_standalone:
        run_build(args.parallel_jobs)

        out_sym = pathlib.Path("/tmp")/"litani"/"runs"/"latest"
        out_dir = out_sym.resolve()

        local_copy = pathlib.Path("output")/"latest"
        local_copy.parent.mkdir(exist_ok=True)
        local_copy.symlink_to(out_dir)


if __name__ == "__main__":
    main()

