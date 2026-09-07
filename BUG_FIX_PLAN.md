# WinuxCmd Bug Fix Plan

## Overview
- **Total bugs to fix**: 95 (5 CRITICAL, 25 HIGH, 37 MEDIUM, 28 LOW)
- **Current test status**: 2360/2366 passed (6 platform limitation failures)
- **Strategy**: Fix CRITICAL → HIGH → MEDIUM → LOW

## Build & Test Commands
```bash
# Build
cd D:/repo/unixwin-winuxcmd && pwsh -NoProfile -Command './scripts/build-with-vs.ps1'
# If LNK1236: rm -rf build-vs && rebuild

# Test
cd build-vs/tests && ./winuxcmd-tests.exe

# Commit
git add -A && git commit -m "fix(command): description"
```

---

## Phase 1: CRITICAL Bugs (5)

### 1.1 ls.cpp - Empty directory error
- **Location**: src/commands/ls.cpp ~line 2597
- **Problem**: Empty dir returns "cannot access" error
- **Fix**: Return success with no output for empty directories
- **File**: src/commands/ls.cpp

### 1.2 ls.cpp - Symlink loop detection
- **Location**: src/commands/ls.cpp recursive listing
- **Problem**: -R -L follows symlinks without loop detection → stack overflow
- **Fix**: Add visited path set to detect cycles
- **File**: src/commands/ls.cpp

### 1.3 date.cpp - --set UTC vs local time
- **Location**: src/commands/date.cpp ~line 677-701
- **Problem**: SetSystemTime expects local time, conversion logic may be wrong
- **Fix**: Verify FileTimeToLocalFileTime + SetSystemTime conversion
- **File**: src/commands/date.cpp

### 1.4 stty.cpp - Negative settings rejected
- **Location**: src/commands/stty.cpp validate_settings
- **Problem**: -icanon, -isig rejected as unrecognized
- **Fix**: Ensure normalized name check handles negatives correctly
- **File**: src/commands/stty.cpp

### 1.5 stty.cpp - -F/--file device ignored
- **Location**: src/commands/stty.cpp ~line 422-427
- **Problem**: Device path checked but never used
- **Fix**: Open device handle and use it instead of stdin
- **File**: src/commands/stty.cpp

---

## Phase 2: HIGH Bugs (25)

### 2.1 mv.cpp - -I never prompts on recursive
- **Location**: src/commands/mv.cpp ~line 567-578
- **Problem**: -I only prompts for >3 files, not recursive dirs
- **Fix**: Add prompt when moving directories recursively
- **File**: src/commands/mv.cpp

### 2.2 mv.cpp - Cross-device follows reparse points
- **Location**: src/commands/mv.cpp ~line 462-498
- **Problem**: Fallback follows symlinks instead of copying them
- **Fix**: Check reparse points and handle separately
- **File**: src/commands/mv.cpp

### 2.3 install.cpp - Error messages to stdout
- **Location**: src/commands/install.cpp
- **Problem**: Some errors use safePrint instead of safeErrorPrint
- **Fix**: Change all error messages to stderr
- **File**: src/commands/install.cpp

### 2.4 ls.cpp - --hide bypassed by -a/-A
- **Location**: src/commands/ls.cpp
- **Problem**: -a should show all files including hidden
- **Fix**: When -a set, don't apply hide pattern
- **File**: src/commands/ls.cpp

### 2.5 comm.cpp - Unknown options become filenames
- **Location**: src/commands/comm.cpp
- **Problem**: Unknown options silently treated as filenames
- **Fix**: Validate options, reject unknown ones
- **File**: src/commands/comm.cpp

### 2.6 date.cpp - -u doesn't affect --date parsing
- **Location**: src/commands/date.cpp
- **Problem**: -u should affect timezone in date parsing
- **Fix**: When -u set, parse as UTC instead of local
- **File**: src/commands/date.cpp

### 2.7 date.cpp - Natural language dates not supported
- **Location**: src/commands/date.cpp parse_date_argument
- **Problem**: GNU supports "next monday", "yesterday", etc.
- **Fix**: Add more natural language patterns
- **File**: src/commands/date.cpp

### 2.8 od.cpp - --strings semantics wrong
- **Location**: src/commands/od.cpp ~line 679-703
- **Problem**: --strings should output strings of N+ graphic chars
- **Fix**: Verify string_type format handling
- **File**: src/commands/od.cpp

### 2.9 od.cpp - Format short-options fixed order
- **Location**: src/commands/od.cpp
- **Problem**: Format options should follow CLI order
- **Fix**: Preserve option appearance order
- **File**: src/commands/od.cpp

### 2.10 stty.cpp - SetConsoleMode return values unchecked
- **Location**: src/commands/stty.cpp apply_setting
- **Problem**: SetConsoleMode failures not properly handled
- **Fix**: Check all SetConsoleMode return values
- **File**: src/commands/stty.cpp

---

## Phase 3: MEDIUM Bugs (37)
- Distributed across all commands
- Includes: error message format, missing validation, silent failures
- Strategy: Fix as encountered during testing

## Phase 4: LOW Bugs (28)
- Distributed across all commands
- Includes: dead code, minor formatting, error message details
- Strategy: Fix when time permits

---

## I18N Updates Required
- **dd.cpp**: Add I18N keys for error messages
  - "dd: invalid status value"
  - "dd: unsupported iflag flag"
  - "dd: unsupported oflag flag"
- **Repository**: D:/repo/winuxcmd-i18n/catalogs/zh-CN/catalog.json

## ISSUE Data Update
- Fetch unixwin/WinuxCmd open issues from GitHub API
- Save to K:/coreutils-issues/winuxcmd_open/
- Generate classification report
- Update BUG_FIX_STATUS.md

---

## Progress Tracking

| Phase | Bugs | Fixed | Remaining |
|-------|------|-------|-----------|
| CRITICAL | 5 | 3 | 2 (date --set, stty negative - may be outdated) |
| HIGH | 25 | 9 | 16 |
| MEDIUM | 37 | 0 | 37 |
| LOW | 28 | 0 | 28 |
| **Total** | **95** | **12** | **83** |

## Fixed Bugs (This Session)
1. ✅ ls.cpp - Empty directory error
2. ✅ ls.cpp - Symlink loop detection (normalize path)
3. ✅ stty.cpp - -F/--file device ignored (open device handle)
4. ✅ mv.cpp - -I never prompts on recursive
5. ✅ ls.cpp - --sort=type support (previous)
6. ✅ date.cpp - month/year/fortnight units (previous)
7. ✅ mkdir.cpp - UNC path handling (previous)
8. ✅ dd.cpp - conv=sync padding + iflag/oflag (previous)
9. ✅ mv.cpp - build_dest_path overflow (previous)
10. ✅ stty.cpp - echoe display (previous)
11. ✅ ls.cpp - --group-directories-first (previous)
12. ✅ expr.cpp - is_null treats 0 as null (previous)

## Notes
- Some audit items may be outdated (e.g., stty negative settings, comm unknown options)
- Cross-device reparse point handling requires Windows symlink API (complex)
- I18N updates for dd.cpp pending
- ISSUE data fetching pending

## Completion Criteria
- [ ] All CRITICAL bugs fixed
- [ ] All HIGH bugs fixed
- [ ] I18N updated for dd.cpp
- [ ] ISSUE data fetched and classified
- [ ] BUG_FIX_STATUS.md updated
- [ ] Test suite passes (2366/2366 or platform limitations documented)
- [ ] This plan file deleted after completion
