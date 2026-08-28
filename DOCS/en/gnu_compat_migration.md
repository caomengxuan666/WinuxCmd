# WinuxCmd GNU Coreutils Migration Guide

This document tracks the migration from partial GNU compatibility to full GNU coreutils 9.11 compatibility.

## Prerequisites

### 1. Upgrade WSL2 GNU coreutils
```bash
# Option A: Compile from source
cd /tmp
curl -LO https://ftp.gnu.org/gnu/coreutils/coreutils-9.11.tar.xz
tar xf coreutils-9.11.tar.xz
cd coreutils-9.11
./configure --prefix=/opt/coreutils-9.11
make -j$(nproc)
make install

# Option B: Use Docker
docker run --rm -it ubuntu:24.04 bash
apt update && apt install -y coreutils
ls --version  # Should show 9.4 or newer
```

### 2. Run Option Extraction Script
```bash
# Extract GNU options from --help
wsl -- bash /path/to/extract_gnu_options.sh > gnu_options.txt

# Extract WinuxCmd options from source
# (Already done - see DOCS/generated/gnu_option_gap_audit.md)
```

## Migration Tasks

### Task 1: Annotate All Options

For each command in src/commands/*.cpp:

1. Find the OPTION() macro array
2. Add annotation comment above each option:
   - // [GNU] - matches GNU coreutils
   - // [EXT] - WinuxCmd Windows extension
   - // [PLACEHOLDER] - Windows placeholder
   - // [DIFFERS] - differs from GNU behavior
3. Update command header documentation

Example:
```cpp
// clang-format off
auto constexpr LS_OPTIONS = std::array{
    // [GNU] -a, --all
    OPTION("-a", "--all", "do not ignore entries starting with ."),
    // [GNU] -l, --long
    OPTION("-l", "--long", "use a long listing format"),
    // [EXT] --kibibytes (WinuxCmd extension)
    OPTION("-k", "--kibibytes", "use 1024-byte blocks"),
};
```

### Task 2: Implement Missing GNU Options

Priority order (by number of missing options):

| Priority | Command | Missing Count |
|----------|---------|---------------|
| P0 | patch | 57 |
| P0 | diff | 41 |
| P0 | ls | 20 |
| P0 | grep | 14 |
| P0 | numfmt | 14 |
| P0 | cp | 11 |
| P0 | sdiff | 11 |
| P0 | stty | 11 |
| P0 | diff3 | 9 |
| P0 | du | 9 |
| P0 | sort | 9 |
| ... | ... | ... |

### Task 3: Add Missing Commands via WPM

Edit src/commands/wpm.cpp to add packages:

```json
{
  "name": "goawk",
  "version": "1.0.0",
  "description": "Go implementation of awk",
  "kind": "external",
  "category": "text",
  "commands": ["awk", "goawk"],
  "artifacts": {
    "windows-x64": {
      "type": "zip",
      "urls": ["https://github.com/benhoyt/goawk/releases/download/v1.0.0/goawk_1.0.0_windows_amd64.zip"]
    }
  }
}
```

### Task 4: Implement Missing Commands

Commands to implement (not available in WPM):

1. **gunzip/zcat** - Wrapper around gzip -d
2. **curl/wget** - Network downloads (complex, consider WPM)
3. **whereis** - Command/manpage/source finder
4. **lscpu** - CPU information
5. **lsmem** - Memory information
6. **ping** - Network connectivity test
7. **ipconfig** - Windows network config (ip equivalent)
8. **tasklist/taskkill** - Windows process management

### Task 5: Update Tests

For each implemented option:
1. Add unit test in tests/unit/<cmd>/
2. Test GNU-compatible behavior
3. Test Windows-specific behavior (if [EXT])
4. Test error handling

### Task 6: Update Documentation

1. Update DOCS/en/commands_implementation_en.md
2. Update DOCS/zh/commands_implementation.md
3. Update DOCS/generated/command_compatibility_matrix.md
4. Add migration notes to DOCS/en/gnu_compat_audit_plan.md

---

## Verification Checklist

- [ ] All options annotated with [GNU]/[EXT]/[PLACEHOLDER]/[DIFFERS]
- [ ] All 402 missing GNU options implemented or clearly marked
- [ ] WPM packages added for awk, tar, gzip, ssh, make, vim
- [ ] Missing P0 commands implemented
- [ ] Unit tests pass for all options
- [ ] Documentation updated
- [ ] Performance benchmarks acceptable
- [ ] Backward compatibility verified

---

## Rollback Plan

If migration causes issues:

1. Revert git commits
2. Restore from backup branch
3. Report issues in GitHub Issues

---

## References

- GNU coreutils manual: https://www.gnu.org/software/coreutils/manual/
- GNU coreutils NEWS: https://raw.githubusercontent.com/coreutils/coreutils/master/NEWS
- WPM source: https://github.com/unixwin/wpm-source
- WinuxCmd audit: DOCS/generated/gnu_option_gap_audit.md