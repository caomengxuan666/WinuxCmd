#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Build WinuxCmd under a Visual Studio developer environment.

Python equivalent of scripts/build-with-vs.ps1: locates vcvars64.bat via
vswhere, initializes the VS environment inside cmd.exe, then configures and
builds the selected CMake target in one shell session.

Examples:
    python scripts/build.py                          # build winuxcmd-tests
    python scripts/build.py --target winuxcmd        # build the main binary
    python scripts/build.py --skip-configure         # incremental rebuild only
    python scripts/build.py --target winuxcmd-tests --run-tests
"""

from __future__ import annotations

import argparse
import os
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent


def find_vs_env_script() -> str:
    """Locate vcvars64.bat / VsDevCmd.bat using vswhere, then known paths."""
    program_files_x86 = os.environ.get("ProgramFiles(x86)", r"C:\Program Files (x86)")
    vswhere = Path(program_files_x86) / "Microsoft Visual Studio" / "Installer" / "vswhere.exe"
    if vswhere.is_file():
        try:
            result = subprocess.run(
                [
                    str(vswhere),
                    "-latest",
                    "-products",
                    "*",
                    "-requires",
                    "Microsoft.VisualStudio.Component.VC.Tools.x86.x64",
                    "-find",
                    r"VC\Auxiliary\Build\vcvars64.bat",
                ],
                capture_output=True,
                text=True,
                check=False,
            )
            candidates = [line.strip() for line in result.stdout.splitlines() if line.strip()]
            if candidates and Path(candidates[0]).is_file():
                return candidates[0]
        except OSError:
            pass

    program_files = os.environ.get("ProgramFiles", r"C:\Program Files")
    fallbacks = [
        Path(program_files) / "Microsoft Visual Studio" / "18" / "Community" / "VC" / "Auxiliary" / "Build" / "vcvars64.bat",
        Path(program_files) / "Microsoft Visual Studio" / "17" / "Community" / "VC" / "Auxiliary" / "Build" / "vcvars64.bat",
        Path(program_files_x86) / "Microsoft Visual Studio" / "17" / "BuildTools" / "VC" / "Auxiliary" / "Build" / "vcvars64.bat",
    ]
    for candidate in fallbacks:
        if candidate.is_file():
            return str(candidate)
    print("error: Visual Studio environment script not found", file=sys.stderr)
    sys.exit(2)


def quote_cmd(value: str) -> str:
    return '"' + value.replace('"', '\\"') + '"'


def run_cmd(command: str) -> int:
    """Run a command line through cmd.exe without re-quoting it.

    A single string is passed to CreateProcess so the quoting produced by
    quote_cmd() reaches cmd.exe verbatim (a list argument would be re-quoted
    by subprocess.list2cmdline and corrupt paths containing spaces).
    """
    cmd_exe = os.path.join(os.environ.get("SystemRoot", r"C:\Windows"),
                           "System32", "cmd.exe")
    return subprocess.run(f"{cmd_exe} /d /s /c {command}", check=False).returncode


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", default=str(ROOT))
    parser.add_argument("--build-dir", default="build-vs")
    parser.add_argument("--target", default="winuxcmd-tests")
    parser.add_argument("--configuration", default="Debug")
    parser.add_argument("--generator", default="Ninja")
    parser.add_argument("--skip-configure", action="store_true")
    parser.add_argument("--configure-only", action="store_true")
    parser.add_argument("--run-tests", action="store_true",
                        help="run ctest after a successful build")
    parser.add_argument("--cmake-extra-args", nargs="*", default=[])
    parser.add_argument("--vs-env-script", default="")
    args = parser.parse_args()

    root_path = Path(args.root).resolve()
    build_path = root_path / args.build_dir
    vs_env_script = args.vs_env_script or find_vs_env_script()

    steps: list[str] = []
    if not args.skip_configure:
        cmake_args = [
            "-S", str(root_path),
            "-B", str(build_path),
            "-G", args.generator,
            f"-DCMAKE_BUILD_TYPE={args.configuration}",
            *args.cmake_extra_args,
        ]
        steps.append("cmake " + " ".join(quote_cmd(a) for a in cmake_args))
    if not args.configure_only:
        steps.append(
            "cmake --build "
            + quote_cmd(str(build_path))
            + " --target "
            + quote_cmd(args.target)
        )

    if not steps:
        print("Nothing to do. Use --configure-only, omit --skip-configure, or both.")
        return 0

    vs_call = "call " + quote_cmd(vs_env_script)
    script_name = Path(vs_env_script).name.lower()
    if script_name == "vsdevcmd.bat":
        vs_call += " -arch=x64"
    elif script_name == "vcvarsall.bat":
        vs_call += " x64"
    command = vs_call + " && " + " && ".join(steps)

    print(f"Root: {root_path}")
    print(f"BuildDir: {build_path}")
    print(f"Target: {args.target}")
    print(f"VS env: {vs_env_script}")
    print()

    exit_code = run_cmd(command)

    if exit_code == 0 and args.run_tests:
        test_command = "ctest -C " + quote_cmd(args.configuration) + " --output-on-failure ."
        test_shell = (
            "call " + quote_cmd(vs_env_script) + " && cd /d "
            + quote_cmd(str(build_path)) + " && " + test_command
        )
        print("\n=== Running tests ===")
        exit_code = run_cmd(test_shell)

    return exit_code


if __name__ == "__main__":
    sys.exit(main())
