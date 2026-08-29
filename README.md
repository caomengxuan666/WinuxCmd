# WinuxCmd

[English](README.md) | [中文](README-zh.md)

> **The most comprehensive Unix command compatibility layer for Windows.**
> 169 commands. GNU Coreutils + BSD + Cygwin + custom extensions.

![GitHub release (latest by date)](https://img.shields.io/github/v/release/unixwin/WinuxCmd)
![GitHub all releases](https://img.shields.io/github/downloads/unixwin/WinuxCmd/total)
![GitHub stars](https://img.shields.io/github/stars/unixwin/WinuxCmd)
![GitHub license](https://img.shields.io/github/license/unixwin/WinuxCmd)
![Windows Support](https://img.shields.io/badge/platform-Windows-blue)

## What is WinuxCmd?

WinuxCmd is a native Windows executable that provides a comprehensive Unix command-line compatibility layer. It implements **169 commands** with **1827 options**, covering:

- **GNU Coreutils** (83 commands): ls, cat, grep, find, sort, cut, tr, wc, etc.
- **GNU findutils/grep/sed** (3 commands): Full implementations with PCRE2 support
- **BSD tools** (15 commands): cal, column, hexdump, logger, tree, etc.
- **Cygwin/MSYS2** (14 commands): cygpath, dos2unix, unix2dos, etc.
- **Process tools** (13 commands): ps, top, kill, killall, pgrep, pkill, etc.
- **System info** (16 commands): hostname, uname, id, who, free, df, etc.
- **Custom extensions** (10+ commands): wpm, mpicalc, d2u/u2d, etc.

## Quick Start

Download the latest release from GitHub Releases, extract, and add to PATH.

## Key Features

- **169 commands** with GNU-style flags
- **1827 options** implemented
- **98% GNU compatibility** (verified against GNU Coreutils 9.11)
- **WPM package manager** for installing Unix tools
- **Windows-native** behavior, no WSL/MSYS2 required

## Documentation

- [Compatibility Matrix](DOCS/en/command_compatibility_matrix.md)
- [GNU Comparison Report](DOCS/en/gnu_comparison_report.md)
- [Windows Features](DOCS/en/windows_features.md)
- [WPM Guide](DOCS/en/wpm_guide.md)

## Building

    ./scripts/build-with-vs.ps1

## License

MIT License
