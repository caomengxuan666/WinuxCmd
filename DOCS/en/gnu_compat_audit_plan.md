# WinuxCmd GNU Coreutils Compatibility Audit & Migration Plan

Generated: 2026-08-27T06:36:53.674Z

## 1. Current Status

### 1.1 Implemented Commands
- WinuxCmd source commands: 169
- GNU Coreutils coverage: 112/113 (near complete)
- Other GNU tools: grep, sed, find, xargs, diff, diff3, sdiff, cmp, patch (all done)
- Non-GNU commands: 57 (cpio, tree, watch, lsof, ps, top, less, more, etc.)

### 1.2 Comparison Baseline Issue
- Current baseline: GNU coreutils 9.4 (WSL2 Ubuntu 24.04)
- Latest stable: GNU coreutils **9.11** (2026-04-20)
- Versions behind: 7 (9.5 to 9.11)
- ACTION: Download coreutils 9.11 and rebuild comparison baseline

### 1.3 Option Gap Statistics
| Metric | Value |
|--------|------:|
| Commands compared | 113 |
| Identical options | 15 |
| Commands with gaps | 98 |
| GNU options missing in WinuxCmd | **402** |
| WinuxCmd-only options | **854** |

---

## 2. WinuxCmd Extra Options Classification

854 WinuxCmd-only options need classification and annotation:

### 2.1 POSIX Short Option Aliases (NO annotation needed)
GNU coreutils chose long-only options, but POSIX defines short forms.
Examples: -R for --recursive, -f for --force

### 2.2 Windows-Specific Extensions (MUST annotate [EXT])
- --context (SELinux placeholder on Windows)
- --debug (Windows process debugging)
- --progress-bar (Windows only)
- -Z (SELinux context)
- --one-file-system (Windows approximation)

### 2.3 WinuxCmd Custom Extensions (MUST annotate [EXT])
- --repeat, --upper (echo extensions)
- --bsd (sum non-GNU extension)
- --diagnose (tee extension)
- --kibibytes (ls extension)

### 2.4 Naming Differences (MUST annotate [DIFFERS])
Some option names differ from GNU. Each needs review.

### 2.5 Annotation Standard
Use in OPTION() macros and documentation:
- [GNU] - matches GNU coreutils
- [EXT] - WinuxCmd Windows extension
- [PLACEHOLDER] - Windows placeholder implementation
- [DIFFERS] - differs from GNU behavior

---

## 3. Missing Commands

### 3.1 Available in WPM (can integrate directly)
| Missing Cmd | WPM Package | Notes |
|-------------|-------------|-------|
| awk | goawk | Go awk implementation |
| tar | bsdtar | BSD tar |
| gzip | gzip | GNU gzip |
| ssh | openssh | OpenSSH |
| scp | openssh | OpenSSH |
| make | make | GNU make |
| vim/vi | neovim | Neovim |

### 3.2 NOT in WPM (must implement or add to WPM)

**P0 Critical (scripts depend on them):**
- gunzip, zcat - gzip companions
- curl, wget - network downloads

**P1 High (system administration):**
- mount/umount - filesystem mounting
- fdisk/lsblk - disk partitions
- lscpu/lsmem - CPU/memory info
- whereis - find commands/manpages/source
- su/sudo/passwd - privilege management
- crontab - scheduled tasks
- systemctl - service management
- ip/ping - networking
- rsync - remote sync

**P2 Medium (development):**
- gcc/g++ - compilers
- ar/ranlib/nm/strip/objdump/readelf - binary tools
- strace/ltrace - tracing

**P3 Lower:**
- bc/dc - calculators
- ed - line editor
- iconv - encoding conversion
- setfacl/getfattr/setfattr - ACL/extended attributes
- lspci/lsusb - hardware info

---

## 4. GNU Coreutils 9.5-9.11 New Features

| Version | Command | New Content |
|---------|---------|-------------|
| 9.11 | cut | -w/--whitespace-delimited, -O, -F aliases, multi-byte |
| 9.11 | date | --date parses dd.mm.yy format |
| 9.10 | tail | --debug option |
| 9.10 | du | -A short option for --apparent-size |
| 9.10 | stat/tail | nullfs filesystem type |
| 9.9 | numfmt | --unit-separator=SEP |
| 9.8 | basenc | --base58 |
| 9.8 | cksum | -a sha3, -a sha2 |
| 9.8 | nproc | cgroup v2 CPU quota support |
| 9.8 | fold | multi-byte support, -c option |
| git HEAD | env | --env0-from=FILE |
| git HEAD | uname | -A/--all-labeled |

---

## 5. Execution Plan

### Phase 0: Infrastructure (1-2 days)
1. Download GNU coreutils 9.11 source
2. Compile/install in WSL2
3. Rerun option extraction script
4. Update comparison data

### Phase 1: Annotation & Documentation (3-5 days)
Dispatch sub-agents:

**Agent-A: Option Annotation**
- Add [GNU]/[EXT]/[PLACEHOLDER]/[DIFFERS] tags to all 169 commands
- Update OPTION() macros in source

**Agent-B: Documentation Update**
- Update DOCS/en/commands_implementation_en.md
- Update DOCS/zh/commands_implementation.md
- Update compatibility matrix

**Agent-C: Test Coverage**
- Write tests for 402 missing options
- Verify existing option GNU compatibility

### Phase 2: Missing Options (5-10 days)
**Batch 1 - P0 commands (worst gaps):**
- patch (57 missing options)
- diff (41 missing options)
- ls/dir/vdir (20-29 missing options)
- grep (14 missing options)
- numfmt (14 missing options)

**Batch 2 - P0 commands (medium gaps):**
- cp (11 missing), sdiff (11), stty (11)
- diff3 (9), du (9), sort (9)

**Batch 3 - P1 commands:**
- All commands missing 1-2 options

### Phase 3: Missing Commands (10-20 days)
**Agent-D: WPM Integration**
- Add goawk, bsdtar, gzip, openssh, make, neovim to WPM list
- Create wrapper scripts/aliases

**Agent-E: Self-Implementation**
- curl/wget - network downloads (or add to WPM)
- gunzip/zcat - gzip companions
- whereis - command finder
- lscpu/lsmem - system info

**Agent-F: Windows-Specific Commands**
- mount/umount - Windows volume mounting
- ping - Windows ping
- ipconfig - Windows network config (ip equivalent)
- tasklist/taskkill - Windows process management

### Phase 4: Testing & Validation (5-10 days)
1. Run full GNU compatibility test suite
2. Compare actual output with GNU behavior
3. Performance benchmarks
4. Final documentation review

---

## 6. Agent Task Assignment

| Agent | Task | Est. Time |
|-------|------|-----------|
| Agent-A | Option annotation (169 commands) | 3 days |
| Agent-B | Documentation update | 2 days |
| Agent-C | Test coverage | 5 days |
| Agent-D | WPM integration | 3 days |
| Agent-E | New command implementation | 10 days |
| Agent-F | Windows-specific commands | 5 days |

---

## 7. Risks & Notes

1. **Windows/Linux semantic differences**
   - File permission model differs (chmod/chown are approximations)
   - Symlinks require admin privileges
   - Process signal mechanism differs (kill Windows implementation)

2. **Performance considerations**
   - Some commands (ls) already have 2999 LOC
   - Balance feature completeness with binary size

3. **Backward compatibility**
   - New options must not break existing behavior
   - [EXT] extension options must remain available

---

## 8. Success Criteria

1. All GNU coreutils 9.11 options have implementation or clear [PLACEHOLDER] annotation
2. All WinuxCmd-only options annotated as [EXT] or [DIFFERS]
3. Missing P0 commands (awk, tar, gzip, etc.) available via WPM or self-implemented
4. Complete test coverage
5. Updated documentation reflects actual compatibility state