#!/bin/bash
# Run GNU tests with WinuxCmd wrappers for comparison

export srcdir=/mnt/j/gnu-coreutils/coreutils-9.11/tests
export abs_top_builddir=/mnt/j/gnu-coreutils/coreutils-9.11
export abs_top_srcdir=/mnt/j/gnu-coreutils/coreutils-9.11

# Prepend WinuxCmd wrappers to PATH
export PATH=/tmp/winuxcmd-wrappers:$PATH

cd /mnt/j/gnu-coreutils/coreutils-9.11

echo "=== Testing with WinuxCmd wrappers ==="
echo "PATH head: $(echo $PATH | cut -d: -f1)"
echo "cat version: $(cat --version 2>&1 | head -1)"
echo "echo version: $(echo --version 2>&1 | head -1)"
echo ""

# Run tests for each command group
CMDS="cat head tail sort uniq cut tr wc ls cp mv rm mkdir"

for cmd in $CMDS; do
    echo "=== Testing $cmd ==="
    test_dir="tests/$cmd"
    if [ -d "$test_dir" ]; then
        # Run .sh tests
        for test_file in "$test_dir"/*.sh; do
            if [ -f "$test_file" ]; then
                test_name=$(basename "$test_file")
                echo -n "  $test_name: "
                timeout 30 bash "$test_file" >/dev/null 2>&1
                rc=$?
                if [ $rc -eq 0 ]; then
                    echo "PASS"
                elif [ $rc -eq 77 ]; then
                    echo "SKIP"
                else
                    echo "FAIL (exit=$rc)"
                fi
            fi
        done
        # Run .pl tests
        for test_file in "$test_dir"/*.pl; do
            if [ -f "$test_file" ]; then
                test_name=$(basename "$test_file")
                echo -n "  $test_name: "
                timeout 30 perl -Itests -MCuSkip -MCoreutils "$test_file" >/dev/null 2>&1
                rc=$?
                if [ $rc -eq 0 ]; then
                    echo "PASS"
                elif [ $rc -eq 77 ]; then
                    echo "SKIP"
                else
                    echo "FAIL (exit=$rc)"
                fi
            fi
        done
    else
        echo "  (no test directory)"
    fi
    echo ""
done
