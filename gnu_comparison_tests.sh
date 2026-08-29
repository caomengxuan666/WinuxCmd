#!/bin/bash
# GNU coreutils vs WinuxCmd comparison test suite

WCBIN="D:\\repo\\unixwin-winuxcmd\\build-vs\\usr\\bin"
WCDIR="D:\\repo\\unixwin-winuxcmd\\test_gnu"
GNUBIN="/mnt/j/gnu-coreutils/coreutils-9.11/src"
TESTDIR="/mnt/d/repo/unixwin-winuxcmd/test_gnu"
TMPDIR="/tmp/gnu_vs_winx"

mkdir -p "$TMPDIR"

PASS=0
FAIL=0
SKIP=0
TOTAL_TESTS=0

compare_output() {
    local name="$1"
    local gnu_out="$2"
    local wcx_out="$3"
    
    # Normalize paths
    local gnu_clean=$(echo "$gnu_out" | sed 's|/mnt/d/repo/unixwin-winuxcmd/test_gnu/||g')
    local wcx_clean=$(echo "$wcx_out" | sed 's|D:\\repo\\unixwin-winuxcmd\\test_gnu\\||g; s|D:/repo/unixwin-winuxcmd/test_gnu/||g; s|D:\\repo\\unixwin-winuxcmd\\test_gnu/||g; s|/mnt/d/repo/unixwin-winuxcmd/test_gnu/||g' | tr -d '\r')
    
    # Also normalize file size in ls -l output
    gnu_clean=$(echo "$gnu_clean" | sed 's/  */ /g; s/^ //; s/ $//')
    wcx_clean=$(echo "$wcx_clean" | sed 's/  */ /g; s/^ //; s/ $//')
    
    TOTAL_TESTS=$((TOTAL_TESTS+1))
    if [ "$gnu_clean" = "$wcx_clean" ]; then
        PASS=$((PASS+1))
        echo "  $name: PASS"
    else
        FAIL=$((FAIL+1))
        echo "  $name: FAIL"
        echo "    GNU: $(echo "$gnu_clean" | head -3)"
        echo "    WCX: $(echo "$wcx_clean" | head -3)"
    fi
}

compare_cmd() {
    local name="$1"
    local gnu_cmd="$2"
    local wcx_cmd="$3"
    
    gnu_out=$(eval "$gnu_cmd" 2>&1)
    wcx_out=$(eval "$wcx_cmd" 2>&1)
    compare_output "$name" "$gnu_out" "$wcx_out"
}

echo "=========================================="
echo "GNU Coreutils vs WinuxCmd Comparison Tests"
echo "=========================================="
echo ""

# ============ cat ============
echo "--- cat ---"
compare_cmd "basic" "run_gnu cat input1.txt" ""