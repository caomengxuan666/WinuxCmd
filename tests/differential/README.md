# GNU differential regression tests

This directory contains independent cases for comparing WinuxCmd with a GNU
coreutils oracle. Set `WINUXCMD_BIN` and `GNU_BIN` when they are not on PATH.

The runner is a gate, not a smoke test. It fails for an empty corpus, missing
oracle, unexplained skip, exit-code difference, or output difference. Use
`--results FILE` to retain machine-readable TSV output for baseline comparison.

Each case supports `cmd`, `args`, `stdin`, `setup`, `timeout`, `source`, and
`tags`. `stdin` and `setup` use a following block indented by two spaces.
