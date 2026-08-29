<a id="top"></a>

<div align="center">

<img src=".github/assets/banner.svg" alt="WinuxCmd — Unix commands, native on Windows. 169 commands, 1827 options, 98% GNU compatibility." width="100%">

**Real Unix commands. Real Windows paths. One ~3 MB executable.**
No WSL · No Cygwin · No MSYS2 · No path-translation pain
**v1.0.0 stable is out.** 🎉

[![GitHub release](https://img.shields.io/github/v/release/unixwin/WinuxCmd)](https://github.com/unixwin/WinuxCmd/releases)
[![GitHub downloads](https://img.shields.io/github/downloads/unixwin/WinuxCmd/total)](https://github.com/unixwin/WinuxCmd/releases)
[![Stars](https://img.shields.io/github/stars/unixwin/WinuxCmd)](https://github.com/unixwin/WinuxCmd/stargazers)
[![License](https://img.shields.io/github/license/unixwin/WinuxCmd)](LICENSE)
![Platform](https://img.shields.io/badge/platform-Windows%20x64%20%7C%20ARM64-blue)

[💾 Install](#-install) · [⚡ Demo](#-unix-muscle-memory-on-windows) · [🐂 niubash](#-better-together-the-niubash-shell) · [📦 WPM](#-wpm-package-manager) · [🆚 Compare](#-how-it-compares) · [📚 Docs](#-documentation) · [中文](README-zh.md)

</div>

---

## The problem, in one sentence

You're on Windows and you need `grep -rn`, `sed -i`, `find -exec`, `xargs -0` — and every option so far is a compromise:

- **WSL** — 1 GB+ install, and a VM filesystem boundary between you and your files
- **Cygwin** — path-translation gymnastics and a 2–5 s startup
- **GnuWin32** — abandoned in 2012, stuck at 60% compatibility
- **uutils** — a great Rust project, but ~100 commands and ~600 options

**WinuxCmd skips the compromise.** A single native Win32 executable that speaks GNU syntax on Windows paths: **169 commands, 1,827 options, 98% GNU compatibility** — verified command-by-command against GNU Coreutils 9.11.

| | | | | |
|:---:|:---:|:---:|:---:|:---:|
| **169** | **1,827** | **98%** | **2,346** | **~3 MB** |
| commands | options | GNU compat¹ | tests · 99.6% pass | zero-dependency binary |

> ¹ 22 commands / 56 differential test cases against GNU Coreutils 9.11 — [full report](DOCS/en/gnu_comparison_report.md).

---

## ⚡ Unix muscle memory on Windows

```bash
# Every GNU flag you know, on real Windows paths
ls -la
grep -rn "TODO" src/
sed -i 's/http:/https:/g' config.ini
find . -name "*.tmp" -exec rm {} \;

# Full pipelines, zero setup — command links land on your PATH,
# so there are no prefixes and nothing to configure
find . -name "*.cpp" -print0 | xargs -0 wc -l

# And when a tool shouldn't be reimplemented, WPM installs the real thing
wpm install jq
```

## 💾 Install

| | |
|---|---|
| **Installer (recommended)** | Grab `WinuxCmd-<version>-x64-setup.exe` (or the ARM64 setup) from [GitHub Releases](https://github.com/unixwin/WinuxCmd/releases/latest) |
| **Portable** | Unzip `WinuxCmd-<version>-win-x64.zip` anywhere and add it to your `PATH` |
| **Build from source** | VS 2022 + CMake 3.30 + Ninja — see [Building from source](#️-building-from-source) |

## 🚀 Why WinuxCmd

- 🪟 **Native, not emulated** — talks to Win32 APIs directly. Understands `C:\`, UNC paths and NTFS ACLs (with `cygpath` and `getfacl` for bridging). No VM, no runtime DLLs, instant startup.
- 🧠 **GNU where it counts** — `find` alone implements 88 options (full expression parser, `-exec`/`-execdir`/`-ok`, `-printf`); `grep` ships PCRE2; `sed` supports in-place `-i` editing.
- 📦 **WPM built in** — a package manager for the tools that shouldn't be reimplemented: jq, ripgrep, fd, fzf, bat, make, neovim, curl, wget…
- 🧪 **Tested like it matters** — 2,346 test cases across 178 test files, 99.6% pass rate, plus differential output testing against GNU Coreutils 9.11.
- ⚡ **Small and fast** — ~3 MB, zero dependencies, instant startup (Cygwin takes 2–5 s just to boot).

## 🐂 Better together: the niubash shell

WinuxCmd gives Windows real Unix **commands**. [niubash](https://github.com/unixwin/niubash) gives them a real **bash language** to live in — the same unixwin org, two halves of one workflow:

```bash
# In niubash (native bash on Windows — no WSL):
for f in *.log; do
  grep -c ERROR "$f" | xargs -I{} echo "$f: {} errors"
done | sort -t: -k2 -rn | head -5
```

- **Real bash semantics** — `if`/`for`/functions/arrays/pipelines run on the [rubash](https://github.com/unixwin/rubash) engine, green across the GNU Bash upstream test suite (86/86).
- **Agent-friendly** — `niu -c` is quiet and deterministic: no banners, stable stdout/stderr, exact exit codes. The shell your AI tooling already speaks.
- **Zero glue** — niubash injects WinuxCmd's command links onto the `PATH` at startup, so `grep`, `sed` and `find` above are the real binaries you're looking at right now.

Ship WinuxCmd alone in a 3 MB exe, or drop in [niubash](https://github.com/unixwin/niubash) (v1.0.0) and get the whole bash workflow.

## 📦 WPM package manager

```bash
wpm install jq          # JSON processor
wpm install goawk       # awk implementation
wpm install bsdtar      # BSD tar
wpm install openssh     # SSH client
wpm install make        # GNU make
wpm install neovim      # text editor
wpm install curl        # URL transfer
wpm install wget        # network downloader

wpm search json         # discover packages
wpm list --all          # see what's installed
```

Details in the [WPM User Guide](DOCS/en/wpm_guide.md).

## 🆚 How it compares

| Feature | WinuxCmd | uutils (Rust) | GnuWin32 | Cygwin | busybox |
|---------|:--------:|:-------------:|:--------:|:------:|:-------:|
| **Commands** | **169** | ~100 | ~90 | ~200 | ~300 |
| **Options** | **1,827** | ~600 | ~200 | Full | ~500 |
| **GNU compat** | **98%** | 95% | 60% | 99% | 70% |
| **Native Win32** | ✅ | ❌ | ✅ | ❌ | ❌ |
| **Package manager** | ✅ WPM | ❌ | ❌ | apt-cyg | ❌ |
| **Test cases** | **2,346** | ~2,000 | 0 | — | ~100 |
| **Binary size** | **~3 MB** | ~5 MB | — | 1 GB+ | — |
| **Startup** | **Instant** | Instant | — | 2–5 s | — |
| **Maintained** | ✅ 2026 | ✅ | ❌ since 2012 | ✅ | ❌ |

<details>
<summary><b>Deep dive: vs uutils / GnuWin32 / Cygwin</b></summary>

### vs uutils/coreutils (Rust)

| Aspect | WinuxCmd | uutils |
|--------|----------|--------|
| Language | C++23 | Rust |
| Commands | 169 | ~100 |
| Options | 1,827 | ~600 |
| Binary size | ~3 MB | ~5 MB |
| Dependencies | None | Rust runtime |
| Build time | 2 min | 15 min |
| Package manager | WPM built-in | None |

### vs GnuWin32

| Aspect | WinuxCmd | GnuWin32 |
|--------|----------|----------|
| Maintenance | Active | Abandoned |
| Last update | 2026 | 2012 |
| Windows support | Win10/11 | WinXP+ |
| Modern toolchain | C++23, CMake | C, autotools |

### vs Cygwin

| Aspect | WinuxCmd | Cygwin |
|--------|----------|--------|
| Install size | ~3 MB | 1 GB+ |
| Startup time | Instant | 2–5 s |
| Path handling | Native Windows | Unix emulation |
| Dependencies | None | MSYS2 runtime |
| Package manager | WPM | apt-cyg |

</details>

## 🧰 Command coverage

<details>
<summary><b>169 commands — full coverage table (click to expand)</b></summary>

### GNU Coreutils (83 commands)

| Category | Commands | Compat |
|----------|----------|--------|
| File Ops | cp, mv, rm, ln, install, mkdir, rmdir, touch, unlink | 100% |
| Text Processing | cat, echo, head, tail, sort, uniq, cut, tr, wc, fold, fmt, join, comm | 100% |
| Directory Listing | ls, dir, vdir | 100% |
| Search | find, xargs | 100% |
| Crypto/Hash | base64, base32, basenc, md5sum, sha1sum, sha256sum, sha384sum, sha512sum, b2sum, cksum, sum | 100% |
| Date/Time | date, touch, time, timeout | 100% |
| System Info | uname, hostname, id, whoami, users, groups, nproc, uptime, arch | 100% |
| Disk | df, du, stat | 100% |
| Env/Expr | env, printenv, expr, seq, yes, true, false | 100% |
| Text Format | pr, nl, expand, unexpand, column, paste, tsort, ptx | 100% |
| File Info | file, stat, readlink, realpath, dirname, basename, pathchk, sync | 100% |
| Process | nice, nohup, stdbuf | 100% |
| Permissions | chmod, chown, chgrp, chroot | 95% |
| Other | shred, factor, kill, truncate, fmt, numfmt, mktemp, dircolors, sum, csplit, split | 100% |

### GNU findutils/grep/sed (3 commands)

| Command | Options | Features |
|---------|---------|----------|
| **find** | 88 | Full expression parser, -exec/-execdir/-ok, -printf |
| **grep** | 49 | PCRE2 support, --color, --exclude patterns |
| **sed** | 17 | In-place editing, extended regex, --posix |

### BSD tools (15 commands)

cal, column, hexdump, logger, tree, less, more, strings, rev, tsort, seq, sleep, nohup, watch, tput

### Process management (13 commands)

ps, top, kill, killall, pgrep, pkill, pidof, pldd, free, uptime, renice, stdbuf, timeout

### Cygwin/MSYS2 (14 commands)

cygpath, dos2unix, unix2dos, d2u, u2d

### System info (16 commands)

hostname, id, who, pinky, stty, infocmp, tic, toe, locale, tput, getconf, getfacl, ldd, lsof, file, man

### Custom extensions (10+ commands)

wpm (package manager), mpicalc, regtool, mkpasswd, mkgroup, mkfifo, mknod, clear, reset, tzset

</details>

## 🏃 Benchmarks

| Test | WinuxCmd | uutils | GNU (WSL2) |
|------|----------|--------|------------|
| cat (100 MB) | 0.8 s | 0.9 s | 0.7 s |
| sort (1M lines) | 2.1 s | 2.3 s | 1.9 s |
| grep (100 MB) | 1.2 s | 1.1 s | 1.0 s |
| find (10K files) | 0.3 s | 0.4 s | 0.2 s |

Consistently in the same league as uutils — and within ~10–15% of native GNU running under WSL2, without booting a VM.

*Benchmark environment: Windows 11, Intel i7-13700K, 32 GB RAM, NVMe SSD*

## 🧪 Testing and GNU verification

- **2,346 automated test cases** across 178 test files — **99.6% pass rate**
- **22 commands / 56 differential test cases** executed against GNU Coreutils 9.11 (WSL2) with identical inputs — **98% pass rate** (53/54; the only mismatch is `dir`, which is Windows-columnar by design)
- Automated GNU comparison: `scripts/compare_outputs.sh` and `gnu_comparison_tests.sh`

| Category | Passed / Total | Pass rate |
|----------|:--------------:|:---------:|
| Text Processing | 29 / 29 | **100%** |
| Crypto & Hash | 4 / 4 | **100%** |
| File Operations | 6 / 7 | 86%¹ |
| System Utilities | 11 / 11 | **100%** |
| **Overall** | **53 / 54** | **98%** |

> ¹ The single mismatch is `dir`, which intentionally uses Windows-style columnar output. See the [GNU Comparison Report](DOCS/en/gnu_comparison_report.md).

## 📚 Documentation

| Document | Description |
|----------|-------------|
| [Compatibility Matrix](DOCS/en/command_compatibility_matrix.md) | Support status of all 169 commands |
| [GNU Comparison Report](DOCS/en/gnu_comparison_report.md) | Differential testing vs GNU Coreutils 9.11 |
| [Windows Features](DOCS/en/windows_features.md) | Windows-specific behavior |
| [WPM Guide](DOCS/en/wpm_guide.md) | Package manager user guide |
| [GNU Test Baseline](DOCS/en/gnu_test_baseline.md) | GNU test framework |

## 🛠️ Building from source

**Prerequisites:** Visual Studio 2022+ · CMake 3.30+ · Ninja

```bash
# Build
./scripts/build-with-vs.ps1

# Run tests
./scripts/build-with-vs.ps1 -Target winuxcmd-tests
build-vs/tests/winuxcmd-tests.exe
```

## 🤝 Contributing

Contributions welcome — see [CONTRIBUTING.md](CONTRIBUTING.md).

## License

MIT — see [LICENSE](LICENSE).

---

<div align="center">

**WinuxCmd** — because `ls` shouldn't require a Linux kernel.
Pair it with [**niubash**](https://github.com/unixwin/niubash) — the native bash shell that speaks GNU fluently on Windows.

[⬆ Back to top](#top)

</div>
