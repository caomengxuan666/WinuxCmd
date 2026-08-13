# GNU differential regression tests

This directory contains independent cases for comparing WinuxCmd with a GNU
coreutils oracle. Set `WINUXCMD_BIN` and `GNU_BIN` when they are not on PATH.

Each case supports `cmd`, `args`, `stdin`, `setup`, `timeout`, `source`, and
`tags`. `stdin` and `setup` use a following block indented by two spaces.
