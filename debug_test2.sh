#!/bin/bash
cd /mnt/j/gnu-coreutils/coreutils-9.11
export srcdir=/mnt/j/gnu-coreutils/coreutils-9.11
export abs_top_builddir=/mnt/j/gnu-coreutils/coreutils-9.11
export abs_top_srcdir=/mnt/j/gnu-coreutils/coreutils-9.11
export PATH=/tmp/winuxcmd-wrappers:$PATH

echo "=== Testing cat-self ==="
bash -x tests/cat/cat-self.sh 2>&1 | head -50
echo "Exit code: $?"
