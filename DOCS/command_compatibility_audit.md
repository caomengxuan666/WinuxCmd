# Command Compatibility Audit

This note tracks the clean-room GNU compatibility work for regex-heavy
commands. Do not vendor GNU grep or gnulib sources into this MIT tree unless
the project intentionally accepts GPL licensing for the affected deliverable.

## Upstream Source Review

This audit is anchored to pulled upstream source, not option-name guessing.
The current local references are:

- GNU grep `8017d31`: `src/grep.c`.
- GNU sed `31df9f7`: `sed/execute.c`, `sed/compile.c`.
- GNU coreutils `c4bf1d4`: `src/ls.c`, `src/dircolors.c`, `src/expr.c`, `src/nl.c`,
  `src/tac.c`, `src/split.c`, `src/csplit.c`, `src/stat.c`,
  `src/realpath.c`, `src/readlink.c`, `src/join.c`, `src/shuf.c`,
  `src/timeout.c`, `src/unlink.c`, `src/od.c`, `src/dd.c`, `src/pr.c`,
  src/chmod.c, src/install.c, src/mktemp.c, src/df.c, src/kill.c,
  src/chown.c, src/chown-core.c, src/touch.c, src/cat.c, src/tee.c,
  src/wc.c, src/head.c, src/tail.c, src/sort.c, src/cut.c,
  src/uniq.c, src/comm.c, src/paste.c, src/uname.c, src/nproc.c,
  src/tty.c, src/whoami.c, src/sleep.c, src/sync.c, src/hostid.c,
  src/logname.c, src/groups.c, src/users.c, src/hostname.c, src/id.c, and src/nice.c.
- GNU coreutils `c4bf1d4`: `src/test.c` for `test` and `[` parsing.
- GNU findutils `eaa3ba5`: `find/parser.c`, `find/pred.c`,
  `find/ftsfind.c`, and `xargs/xargs.c`.
- GNU diffutils: `src/cmp.c` and `lib/cmpbuf.c`.
- GNU coreutils checksum family: `src/cksum.c`,
  `src/coreutils-sha224sum.c`, `src/coreutils-sha384sum.c`, and
  `src/blake2/b2sum.c`.
- BLAKE2 reference implementation bundled by GNU coreutils:
  `src/blake2/blake2b-ref.c`.
- less: `main.c`, `command.c`, `forwback.c`, `input.c`, and `line.c`.
- util-linux: `text-utils/more.c` for `more`, `text-utils/rev.c` for `rev`,
  and `text-utils/column.c` for `column`.
- Cygwin: `winsup/utils/cygpath.cc`.
- Cygwin/newlib fa7b0cd: winsup/utils/kill.cc, cygwin/signal.h,
  and strsig.cc for Windows signal numbering and output shape.
- man-db: `src/man.c`.
- GNU binutils `676d6209`: `binutils/strings.c`.
- procps-ng 619562d: src/kill.c and local/signals.c for kill -l,
  -L, old-style -SIGNAL, and -q/--queue behavior cross-checking;
  `src/ps/parser.c` and `src/ps/output.c` for `ps` parser/output shape.
- ncurses: `progs/infocmp.c` for terminfo entry loading, decompilation,
  comparison mode, and capability dumping.
- dos2unix: `dos2unix/dos2unix.c` and `dos2unix/common.c` for stream
  conversion, old-file temporary-file replacement, CRLF/LF handling, and
  alias mode wiring.
- uutils coreutils `d169a9a`: cross-check source for Rust command behavior
  when GNU code paths are platform-heavy.

Clean-room rule: read upstream structure and observable behavior, then
reimplement with WinuxCmd helpers and local modules. Do not paste GPL source
into this tree.

Source observations already applied:

- GNU grep's `grepbuf` scanner is driven by a compiled matcher callback
  (`EGexecute`/`Fexecute`), page-sized buffered reads via `good_readsize`, and
  scanner-level handling for context, count, quiet, file-name, and
  only-matching modes.
- GNU grep fixed-string mode compiles all `-F`/`-f` literals into a
  prepared `kwset` (`kwsearch.c`) and searches each buffer with `Fexecute`,
  rather than trying every pattern separately on every input line.
- GNU sed keeps records in reusable `struct line` buffers. `read_file_line`
  removes the input delimiter and records `line.chomped`; `output_line` writes
  the delimiter only when that flag is set. This is why final unterminated
  input records must remain unterminated after ordinary substitutions.
- GNU sed substitution reuses line buffers (`line`, `s_accum`, `buffer`) and
  swaps/accumulates rather than repeatedly rebuilding whole-line strings for
  every command step.
- GNU sed `execute.c` implements the `F` command by writing the current
  `input->in_file_name` followed by the active buffer delimiter, after first
  flushing any pending missing-newline state.
- GNU findutils uses a parser table (`parser.c`) to build a predicate tree,
  then evaluates predicates during FTS traversal (`ftsfind.c`). Important P0
  predicates are not just flags: `-perm`, `-samefile`, `-inum`, `-links`,
  owner/group checks, time-reference checks, access checks, `-exec`,
  `-delete`, `-prune`, and `-print0` all participate in expression truth
  values and short-circuiting.
- GNU findutils `pred_xtype` reverses the normal symlink stat mode: when
  traversal normally stats the link itself, `-xtype` stats the target; when
  `-H` or `-L` makes traversal follow the link, `-xtype` checks the link
  itself. Broken target stats fall back to the normal `-type` behavior.
- GNU coreutils `expr` is a recursive-descent evaluator (`eval7` through
  `eval1`) with precedence layers for primary/grouping, keywords, `:`,
  arithmetic, comparisons, `&`, and `|`. `STRING : REGEXP` and `match` compile
  POSIX basic regular expressions and anchor matching at the start of STRING.
- util-linux `rev` reads separator-delimited wide-character records, reverses
  the record body while leaving the separator at the end, supports `-0` for NUL
  separators, and preserves a final unterminated record without inventing a
  newline.
- Cygwin `cygpath` defaults to Unix output when no output type is supplied,
  treats `-u`, `-w`, `-m`, `-t TYPE`, and `-M` as mutually exclusive output
  modes, and delegates `-p` path-list conversion to `cygwin_conv_path_list`
  rather than converting each operand independently.
- GNU coreutils `test.c` treats `[` as the same expression parser as `test`
  after first requiring a closing `]`; expression operators such as `-eq` are
  tokens consumed by `posixtest`, not command-line options.
- man-db `man.c` is a full manual-page lookup/format/display pipeline with
  `argp` option parsing, MANOPT handling, locale/manpath resolution, formatter
  pipelines, and pager integration. WinuxCmd's `man` is intentionally scoped
  to the built-in command registry.
- GNU coreutils `nl.c` builds `print_no_line_fmt` as spaces with length
  `lineno_width + strlen(separator_str)` and writes that prefix for unnumbered
  lines. It does not print the configured separator on unnumbered lines.
- GNU coreutils `tac.c` compiles `-r/--regex` separators with gnulib
  `re_compile_pattern`; this is closer to GNU basic regular expression
  behavior than to ERE. Literal separators remain the common fast path.
- GNU coreutils `split.c` `-l/--lines` writes deterministic suffix files from
  the input prefix, while `csplit.c` regex patterns are compiled internally via
  `re_compile_pattern` and may suppress byte-count output with `-s`.
- GNU coreutils `stat.c` routes `-c/--format` directives such as `%n` and `%s`
  through its own formatter, and `realpath.c` computes relative output after
  canonicalizing the base paths.
- GNU coreutils `readlink.c` shares the canonicalization modes `-f/-e/-m`
  with gnulib `canonicalize_filename_mode`; Windows path spelling differs, so
  stdout parity is not claimed for canonicalized paths yet.
- GNU coreutils `join.c` prints the join field followed by non-join fields
  from file 1 and file 2 using `output_separator`; the default sorted-file
  case is deterministic and directly comparable.
- GNU coreutils `shuf.c` builds a bounded random source with
  `randint_all_new`, then emits either a permutation, reservoir sample, or
  repeated random records. `--random-source` gives a deterministic black-box
  comparison point.
- GNU coreutils `timeout.c` starts the child, arms a timeout, waits for the
  monitored process, and returns the child status unless a timeout maps it to
  `124` without `--preserve-status`.
- GNU coreutils `unlink.c` is intentionally tiny: parse exactly one operand,
  call `unlink`, and report a GNU-shaped diagnostic on failure.
- GNU coreutils `od.c` builds a list of typed print functions from
  `decode_format_string`, applies `-A` by selecting an address formatter, uses
  `-N`/`-j` to bound the dump window, and prints the ASCII trailer only for
  type specs with the `z` suffix.
- util-linux `column.c` defaults table mode to greedy whitespace tokenization
  over tab/space, switches to non-greedy delimiter scanning when `-s` is
  supplied, uses two spaces as the default table output separator, and delegates
  full table rendering/JSON/tree behavior to libsmartcols.
- GNU coreutils `dd.c` parses assignment operands such as `if=`, `of=`,
  `bs=`, `count=`, `skip=`, `seek=`, `iflag=`, `oflag=`, and `status=` before
  running a block-copy loop with special handling for `iflag=fullblock`,
  `conv=noerror`, and sparse/sync/direct flags.
- GNU coreutils `pr.c` is a full paginating printer: option parsing configures
  page ranges, columns, headers, separators, width, double-spacing, merge mode,
  and form-feed handling before `print_page` drives formatted output.
- GNU coreutils `chmod.c` compiles MODE through gnulib `mode_compile` and
  applies the resulting mode with `fchmodat`. On Windows/MSYS, the observable
  file attribute tracks the owner write bit: `chmod 444` and `u-w` set
  ReadOnly, while `chmod 644` and `go-w` keep the file owner-writable.
- GNU coreutils `install.c` defaults non-directory installs to mode `0755`,
  parses `-m/--mode` with the same mode compiler family, compares permissions
  as part of `-C/--compare`, and applies final attributes after copying.
- GNU coreutils `mktemp.c` validates the template X run, creates or dry-runs a
  randomized candidate, and cannot be compared by exact stdout because the
  generated name is intentionally random.
- GNU coreutils `dir` and `vdir` are `ls.c` entry points with different default
  output formats. `vdir --format=single-column` is a stable same-argument
  comparison point; `vdir -1` is not, because GNU `vdir` keeps its long-list
  default in that shape. GNU `ls -R` routes directory sections through
  `print_dir()`, so every recursive directory section, including a single
  command-line directory operand, gets a `name:` header and sections are
  separated by a blank line.
- GNU coreutils `df.c` drives output through `field_data`,
  `decode_output_arg`, `get_field_list`, and `get_header`. `--output` owns its
  own field list and is mutually exclusive with `-i`, `-P`, and `-T`; `df -P`
  uses a `1024-blocks` header unless `POSIXLY_CORRECT` requests 512-byte
  blocks. `PCENT_FIELD` computes `Use%` by ceiling `used * 100 /
  (used + available)`, not by rounding `used / total`.
- GNU binutils `strings.c` uses `STRING_ISGRAPHIC`, `get_char`,
  `unget_part_char`, and `print_strings` to scan 1/2/4-byte encodings,
  rewind multi-byte misses by one byte, optionally prefix filenames, and emit a
  caller-supplied output separator. `-d`/`-T` depend on BFD object-section
  parsing, while `-U` drives Unicode display transforms.
- GNU coreutils kill.c uses operand2sig for -s, -n, and old-style
  -SIGNAL. procps-ng adds the same -l signal conversion shape plus
  -q/--queue. Cygwin kill.cc and signal.h provide the local Windows/MSYS
  signal table used for black-box parity.
- GNU coreutils chown.c drives both chown and chgrp modes through the shared
  chown-core.c path. Its recursive --dereference mode rejects -R without an
  explicit -H or -L traversal choice before processing operands.
- GNU coreutils touch.c routes file timestamp updates through fdutimensat.
  -h/--no-dereference prevents creation, passes AT_SYMLINK_NOFOLLOW, and also
  makes -r read reference timestamps with lstat rather than stat; -f is parsed
  and ignored for BSD compatibility.
- GNU coreutils cat.c is always unbuffered, so -u is intentionally ignored.
  Decorated output is byte/state driven: line numbers are six-wide followed by
  a tab, final unterminated records are not given a synthetic newline, -E shows
  ^M$ for CRLF records, and -s only squeezes repeated newline-only blank lines.
- GNU coreutils tee.c treats -p and bare --output-error as the warn-nopipe
  mode, keeps --output-error=MODE optional, ignores SIGINT for -i, and does
  not treat '-' as stdin/stdout magic for file operands.
- GNU coreutils sort.c models ordering with keyfield structures, comparator
  flags, merge/check modes, and a record delimiter that switches to NUL for
  -z. Check mode reports FILE:LINE: disorder: RECORD, while -C keeps the
  same exit behavior without the diagnostic.
- GNU coreutils cut.c parses exactly one list among -b, -c, -f, and -F.
  -F behaves like fields mode but implies whitespace-delimited input and a
  single-space default output delimiter; --whitespace-delimited accepts the
  optional value trimmed, which ignores leading/trailing blank runs.

- GNU findutils `xargs.c` delegates batching to `buildcmd_control`, with
  mutually exclusive `-n`/`-L`/`-I` families, POSIX-defined exit status
  mapping, optional `-P` parallelism, and command-owned delimiter parsing.
- GNU coreutils `uniq.c` uses independent output state bits for unique
  lines, first repeated lines, and later repeated lines. `--group` is
  mutually exclusive with `-c/-d/-D/-u`, and `--all-repeated` only accepts
  `none`, `prepend`, and `separate` methods.
- GNU coreutils `comm.c` checks ordering as a three-state policy: default
  mode warns only after an unpairable line is seen, `--check-order` makes
  disorder fatal, and `--nocheck-order` disables the check. Repeated
  different `--output-delimiter` values are rejected.
- GNU coreutils `paste.c` parses delimiter escape lists with byte lengths,
  treats `\0` as an empty delimiter, preserves delimiter positions after
  shorter files close in parallel mode, and consumes repeated `-` operands
  from stdin sequentially.

- GNU coreutils `uname.c` drives both `uname` and `arch`: `arch` is the
  machine-hardware-name-only entry point, while `uname -m` selects the same
  field through the normal option mask.
- GNU coreutils `nproc.c` parses `--all` and `--ignore=N` before rejecting
  extra operands. `--ignore` subtracts from the available processor count but
  preserves GNU at-least-one processing-unit output shape.
- GNU coreutils `tty.c` reports non-tty stdin as a failing `not a tty` result;
  `-s/--silent/--quiet` suppresses output while keeping that status.
- GNU coreutils `whoami.c` and `logname.c` are small identity commands: they
  reject operands and print the effective user or login name with one trailing
  newline when the platform can provide it.
- GNU coreutils `hostid.c` prints an eight-hex-digit host identifier. The
  numeric value is platform-specific, so the Release probe compares the stable
  output shape rather than exact bytes.
- GNU coreutils `sleep.c` sums parsed NUMBER/SUFFIX intervals before sleeping,
  and `sync.c` separates global sync from file-specific data/filesystem sync
  modes; the current Release probes cover the stable zero-sleep and no-operand
  sync shapes first.
- GNU coreutils `groups.c` resolves the current user by default and accepts
  user operands, but exact group output is account/database dependent on
  Windows, so it remains source-reviewed without an exact stdout probe.
- GNU coreutils `hostname.c` prints `xgethostname()` with no operands, treats
  one operand as a set-hostname request, and rejects extra operands before
  attempting any platform call.
- GNU coreutils `id.c` enforces one only-mode among `-u`, `-g`, `-G`, and
  `-Z`; rejects `-n`, `-r`, and `--zero` in default format; and prints only
  raw IDs or names for the selected mode. WinuxCmd now maps ordinary Windows
  account SIDs through the MSYS-style `0x30000 + RID` shape for the common
  current-user UID/GID probes.
- GNU coreutils `nice.c` prints current niceness when no command is supplied,
  requires a command when an adjustment is supplied, and accepts both
  `-n/--adjustment=N` and legacy signed numeric adjustment operands.

## Regex Module

Implemented `utils:regex` as a portable, non-`std::regex` engine for current
command needs.

Covered today:

- BRE/ERE literals, `.`, `^`, `$`, groups, alternation, `*`, `+`, `?`, `{m,n}`.
- Bracket classes, ranges, negation, and common POSIX classes such as
  `[[:digit:]]`.
- GNU-style word-boundary tokens `\<` and `\>`.
- ASCII case-insensitive matching.
- Whole-string matching and iterative match enumeration for command scanners.
- Capturing submatches as first-class output for sed-style replacements.
- Pattern backreferences `\1` ... `\9`.
- Common shorthand classes `\d`, `\D`, `\s`, `\S`, `\w`, `\W` for
  compatibility with the previous sed behavior.

Still missing versus a full GNU regex stack:

- Locale-sensitive collation/equivalence classes.
- Multi-byte locale-aware character classes.
- Full GNU DFA/Boyer-Moore scale for large regex scans.




## xargs, uniq, comm, and paste

Updated:

- Rechecked GNU findutils `xargs/xargs.c` and GNU coreutils `uniq.c`,
  `comm.c`, and `paste.c` against the local implementations.
- Fixed `uniq` output flag composition so `-d -u` suppresses all output and
  `-D -u` emits only later repeated records, matching GNU state-bit logic.
- Rejected GNU-invalid `uniq -c -D`, `uniq --group -c`, and
  `uniq --all-repeated=append` combinations instead of producing plausible
  but incompatible output.
- Fixed `comm` default sorting diagnostics: default mode now emits the merged
  output, reports file/input disorder after an unpairable line, and exits
  nonzero; `--nocheck-order` still accepts the same input.
- Fixed `comm` repeated different `--output-delimiter` handling to return the
  GNU-shaped error instead of silently using the last delimiter.
- Added direct tests for the source-led `uniq` combinations and `comm` order
  policy/delimiter behavior.
- Added Release probes for `comm` default unsorted warning, `uniq -d -u`,
  `uniq -D -u`, and the `uniq -c -D` conflict. The existing `xargs` and
  `paste` probes remain green after the batch.

Remaining gaps:

- `xargs` still has Windows-specific process/TTY semantics around `-o`,
  signal handling, and very large command lines that cannot be byte-for-byte
  identical to POSIX fork/exec behavior.
- `comm` comparisons are byte/string ordered under `LC_ALL=C`; full locale
  collation parity remains out of scope until locale-aware comparison is
  shared across text commands.
- `paste` is still whole-file buffered locally, while GNU streams file
  descriptors; large-file memory behavior remains a follow-up performance
  item even though hot output semantics now have coverage.

## tee

Updated:

- Rechecked GNU coreutils `src/tee.c`: `--output-error` has an optional
  MODE argument, and a bare `--output-error` follows the same `warn-nopipe`
  behavior as `-p`.
- Changed WinuxCmd `--output-error` to `OPTIONAL_STRING_TYPE`, so
  `tee --output-error out.txt` keeps `out.txt` as a file operand instead of
  consuming it as a mode string.
- Kept `-i/--ignore-interrupts` wired to `SIGINT` ignore behavior and
  updated stale source comments that still marked it unsupported.
- Added direct tests for bare `--output-error` and inline
  `--output-error=warn-nopipe`. Added a Release isolated state probe proving
  the bare option still creates the output file while matching GNU stdout.

Remaining tee gaps versus GNU coreutils:

- Pipe-specific broken-output semantics still use the local cross-platform
  approximation rather than GNU's `iopoll`/`EPIPE` descriptor pruning loop.
- Diagnostics are close but not fully GNU-quoted for every open/close failure.


## cat

Updated:

- Rechecked GNU coreutils `src/cat.c`: the source explicitly says cat is
  always unbuffered and `-u` is ignored, so WinuxCmd keeps that option as an
  accepted GNU/POSIX compatibility no-op.
- Replaced the option-enabled line processor with a byte/state scanner closer
  to GNU cat's decorated path. Fast passthrough for plain `cat FILE` stays
  streaming and unchanged.
- Fixed numbered output to use GNU's six-wide number plus tab separator, and
  preserved final unterminated lines instead of appending a newline.
- Fixed `-E`/`-vE` CRLF handling so CRLF records render as `^M$` before
  the newline where GNU does.
- Fixed `-s/--squeeze-blank` so only repeated newline-only empty lines are
  squeezed; whitespace-only lines remain content lines.
- Added direct tests for GNU tab numbering, unterminated final lines, CRLF
  show-ends rendering, whitespace-only squeeze behavior, and `-u` ignored
  compatibility. Added a Release black-box probe for `cat -n` on an
  unterminated input record.

Remaining cat gaps versus GNU coreutils:

- Full multibyte locale display behavior and obscure binary terminal edge cases
  remain out of scope for the current scanner.
- The plain passthrough path does not attempt GNU's platform-specific
  splice/copy_file_range optimizations, but remains block-streamed.

## touch

Updated:

- Rechecked GNU coreutils `src/touch.c` before changing behavior. The key
  branch is `-h/--no-dereference`: GNU avoids the create-open path, updates
  the link/reparse point itself, and reads `-r` reference timestamps with
  no-follow semantics.
- Implemented `-h/--no-dereference` with `FILE_FLAG_OPEN_REPARSE_POINT`
  and `OPEN_EXISTING`, so reparse points are updated directly instead of
  silently following their target.
- Updated `-r/--reference` so `touch -h -r link target` uses the link's
  own timestamp when the platform exposes it.
- Added `FILE_FLAG_BACKUP_SEMANTICS` for timestamp handles, which brings
  directory timestamp updates into the same path as ordinary files.
- Added direct tests for fixed directory timestamps, junction no-follow
  updates, symlink no-follow updates where local privileges allow symlink
  creation, and no-follow reference timestamps.
- Added a Release black-box probe for `touch -d ... touch-dir`; the harness
  now supports optional mtime state fingerprints for deterministic timestamp
  probes.

Remaining touch gaps versus GNU coreutils:

- `-f` remains an intentional BSD/GNU compatibility placeholder: accepted and
  ignored.
- GNU's full `parse-datetime` grammar is larger than the current
  common-date parser. ISO-like fixed times, compact numeric timestamps,
  UTC/GMT/Z/offset suffixes, `@epoch`, common relative forms, and `-r` as
  the base for relative `-d` are covered.
- GNU treats a FILE operand of `-` as the file associated with stdout;
  WinuxCmd still treats it as an ordinary path.

## grep

Updated:

- `grep` now uses `utils:regex` instead of `std::regex`.
- Added `egrep` and `fgrep` registry entries; build links create
  `egrep.exe` and `fgrep.exe`.
- Added `-u/--unix-byte-offsets` acceptance.
- Wired `-U/--binary` to preserve CR before LF.
- Added `-f -` pattern-file support from standard input.
- Added a streaming file scanner for non-context paths so large files do not
  have to be decoded into one string.
- Added a fixed-string file fast path for common `grep -F` workloads, including
  `-q`, `-c`, `-n`, `-l/-L`, `-Z`, CRLF handling, and ASCII `-i`.
- Added a clean-room Aho-Corasick matcher for the fixed fast path so large
  `-F -f` pattern sets are compiled once and searched in one pass per line,
  matching GNU grep's `kwset` architecture without vendoring GPL code.
- Recursive matches now emit generic `/` path separators, matching GNU/MSYS
  behavior for slash-style operands.
- Added GNU `-NUM` context shorthand through the shared option parser.
- Extended `GREP_COLORS` handling for `sl=`, `cx=`, and `rv`, including the
  `-v` selected/context line-color swap and context-line `mc=` match color.
- Added `-P/--perl-regexp` through static PCRE2, with JIT disabled by default;
  the shared clean-room BRE/ERE matcher remains the engine for `-G`, `-E`,
  `sed`, `find`, and other POSIX-regex consumers.
- Added regression coverage for GNU word-boundary tokens, GNU BRE escaped
  alternation (`\\|`), POSIX classes, intervals, `-f -`, `-u`, `-U`,
  `egrep`/`fgrep`, fixed-string streaming,
  many-literal `-F -f` matching, recursive `-REIn` extended alternation
  no-match/match behavior for the release-check script shape, and real NUL
  output for `-Z`, `-NUM`, and GNU line-color capabilities. Added a Release
  black-box performance probe for `grep -F -f grep-many-patterns.txt big.txt`.

Remaining grep gaps versus GNU grep:

- Deeper GNU `pcresearch.c` edge cases remain, especially around locale,
  binary/NUL combinations, and exact warning wording.
- Context/colored/only-matching paths still use the richer in-memory scanner;
  the GNU source keeps these concerns inside the scanner, so this needs a
  second streaming pass rather than an output-only patch.

## sed

Updated:

- `sed` substitutions and regex addresses now use `utils:regex`, including
  capture replacement and pattern backreferences.
- Added a literal substitution fast path for safe `s/literal/replacement/`
  scripts, plus a whole-stream path for simple global literal substitutions.
- Corrected GNU sed's `line.chomped` behavior: records are read without the
  delimiter and output only restores the delimiter when the input record had
  one. A final input line without `\n` stays without `\n`.
- Added GNU `F` filename command support for regular file operands and stdin
  (`-`), using the current sed input name and active delimiter.
- Added GNU `0,/RE/` range handling: line address 0 starts active before
  the first input record and closes on the first regex end-address match.
- Added regression coverage for BRE-vs-ERE metacharacter behavior,
  substitution pattern backreferences, no-final-newline preservation,
  `0,/RE/` range behavior, and `F` filename output, plus Release black-box
  probes for `sed '0,/foo/s/foo/XX/'` and
  `sed -n F sed-file-a.txt sed-file-b.txt`.

Remaining sed gaps versus GNU sed:

- Full GNU sed command language is not complete; `e` shell execution, `v`
  version assertions, sandbox/debug modes, and deeper multibyte/locale edge
  cases still need source-led implementation or explicit diagnostics.
- GNU's `0rFILE` read-prepend special case is not implemented yet; the
  aligned line-zero behavior is currently scoped to `0,/RE/` ranges.
- Literal fast paths are intentionally narrow. Anything involving addresses,
  explicit printing, non-default output suppression, non-literal replacements,
  or regex metacharacters falls back to the general pipeline.
- Multi-byte locale behavior still follows the local `utils:regex` limits.

## find

Updated:

- `find` now recognizes and evaluates GNU/POSIX hot predicates
  `-perm`, `-readable`, `-writable`, `-executable`, `-samefile`, `-inum`,
  `-links`, `-user`, `-group`, `-uid`, `-gid`, `-xtype`, and common GNU
  `-newerXY` forms.
- `-perm` implements GNU's octal exact/all/any forms: `MODE`, `-MODE`, and
  `/MODE`. Symbolic modes are still explicitly rejected with a targeted
  diagnostic instead of being silently misinterpreted.
- `-samefile` uses Win32 volume serial + file index, so hard links compare by
  file identity rather than by path text.
- `-inum` compares the same Win32 file-index value used by `%i`; `-links`
  compares the Win32 hard-link count used by `%n`.
- `-user`/`-group` follow GNU's name-or-numeric shape using Win32 SID-derived
  account fields. Numeric fallback compares the RID surfaced by `%U`/`%G`;
  domain-qualified POSIX UID/GID databases are not available on Windows.
- `-uid`/`-gid` support GNU-style numeric comparisons (`N`, `+N`, `-N`) over
  those same SID-derived numeric fields.
- `-newerXY` now supports the GNU valid selector matrix for access (`a`),
  birth/creation (`B`), status-change (`c` via Win32 `ChangeTime`), modify
  (`m`), and literal time (`t`) references. Literal parsing intentionally
  covers fixed ISO-like forms (`YYYY-MM-DD`, optional `HH:MM[:SS]`) and
  `@SECONDS`.
- `-files0-from` reads NUL-delimited starting points from a file or stdin.
- `-xtype` follows GNU's opposite-symlink-resolution rule for the supported
  `f`, `d`, and `l` file types: default traversal checks the symlink target,
  while `-H` command-line symlink roots and `-L` traversal check the link
  itself. Broken target stats fall back to the same self-type behavior used by
  `-type`.
- Added regression coverage for access predicates, executable extension
  handling, octal permission matching, hard-linked `-samefile`,
  `-inum`/`-links`, owner/group predicates, `-newerXY`, `-files0-from`, and
  `-xtype`. Release black-box coverage now includes recursive name/type,
  `-empty -printf`, `-prune -o` branch scoping, and `-xtype` probes against
  GNU findutils.

Remaining find gaps versus GNU findutils:

- `-execdir`/`-okdir`, symbolic `-perm`, full GNU file-type letters beyond
  the currently supported `f`/`d`/`l`, full GNU `parse_datetime`
  natural-language forms for `-newerXt`, and exact Unix user/group database
  semantics still need source-led implementation or explicit placeholder
  diagnostics.
- Traversal is based on `std::filesystem`, not GNU FTS, so some race,
  mount-boundary, and symlink edge cases remain Windows-specific.
- Permission/access predicates use the existing WinuxCmd Windows permission
  approximation shared with `%m`/`%M`, not POSIX ACL evaluation.

## cp, mv, and rm

Updated:

- Rechecked GNU coreutils `src/cp.c`, `src/mv.c`, `src/rm.c`, and
  `src/remove.c`. The local comparison now treats copy, move, and removal
  behavior as a single file-operations batch instead of only basic smoke
  commands.
- Added Release black-box state probes for `cp -R`, `cp -n`, `cp -u`,
  `cp -b -S`, `cp --parents`, `mv` rename, `mv -n`, `mv -u`,
  `mv -b -S`, `rm -r`, `rm -f` on a missing file, `rm -d` on an empty
  directory, and `rm -v` on a single file.
- Fixed `cp --parents` to treat both Windows and slash-style separators as
  path separators when creating intermediate directories, and to normalize
  the preserved source path under the destination. This keeps slash-style
  operands from `winuxsh` aligned with GNU/MSYS behavior.
- Added direct regression coverage for `cp --parents src/nested/file.txt dest`.

Remaining cp/mv/rm gaps versus GNU coreutils:

- Metadata fidelity is still Windows-scoped: ownership, ACL, xattr, SELinux,
  sparse-file, and reflink behavior are not GNU-complete.
- Cross-device move fallback and interactive prompt edge cases still need
  broader source-led parity probes before claiming full GNU equivalence.
- Removal traversal uses the local Windows filesystem layer rather than GNU
  FTS/remove internals, so race diagnostics and permission edge cases remain
  documented platform gaps.

## tr, du, ln, mkdir, and rmdir

Updated:

- Rechecked GNU coreutils `src/tr.c`, `src/du.c`, `src/ln.c`,
  `src/mkdir.c`, and `src/rmdir.c`. The comparison focused on byte-table
  `tr` transforms, FTS-style `du` traversal/accounting, atomic/forced link
  creation, mkdir parent creation, and rmdir parent removal behavior.
- Fixed `ln -v` to use GNU-style verbose link output and removed the local
  summary/progress lines that GNU coreutils does not emit.
- Added direct regression coverage for single and multi-link verbose `ln`
  output, including the no-summary multi-link case.
- Added Release black-box probes for `tr -cs`, `tr -t`, `du -b -c` with
  `--files0-from`, `ln -v`, `ln -b -S`, and
  `mkdir -vp`, and `rmdir --ignore-fail-on-non-empty`.

Remaining tr/du/ln/mkdir/rmdir gaps versus GNU coreutils:

- `tr` remains byte/ASCII scoped for classes and does not implement full
  locale collation, equivalence classes, or very large repeated-character
  constructs from GNU `Spec_list`.
- `du` uses Win32 traversal and file identity approximations rather than
  GNU FTS plus exact device/inode hard-link accounting, so sparse files,
  mount boundaries, and Unix metadata are documented platform gaps.
- `ln` still has Windows-specific symbolic-link privilege behavior, and
  relative symlink plus hard-link-to-symlink modes need broader source-led
  coverage before claiming full GNU parity.
- `mkdir -vp` now reports each newly created ancestor directory; broader
  mode/SELinux metadata remains Windows-scoped.
- `rmdir` parent removal is covered for hot paths, but symlinked path
  components and Unix errno distinctions remain Windows-specific.

## head and tail

Updated:

- Rechecked GNU coreutils `src/head.c` and `src/tail.c`. GNU `head` uses
  seek/copy fast paths for regular-file `-c -NUM` and `-n -NUM`, while GNU
  `tail` scans to the selected start point and then dumps the remainder in
  blocks for `+NUM`/`-n +NUM`.
- Added regular-file seekable fast paths for WinuxCmd `head -c -NUM` and
  `head -n -NUM`, preserving binary-safe output and keeping the UTF BOM/UTF-16
  text-decoding path for normal line mode.
- Fixed obsolete GNU `tail +NUM FILE` parsing. Bare `+5` now maps to
  `tail -n +5` instead of being treated as a file operand; compact suffix forms
  such as `+2l` and `+1b` remain supported.
- Reworked `tail -n +NUM` to skip records by buffered delimiter scanning and
  stream the remainder in blocks instead of printing one line at a time.
- Added direct regression tests for large `head` all-but-last byte/line cases
  and GNU-shaped `tail +5`. Release black-box coverage now includes
  `head -n 1000`, `head -c -4096`, `head -n -200`, `tail -n 1000`,
  `tail -c 4096`, `tail -c +4096`, and `tail +1000` against GNU/MSYS.

Remaining head/tail gaps versus GNU coreutils:

- `tail -f` uses a Windows polling/watch approximation rather than GNU's full
  inotify/poll backend; current tests cover descriptor/name follow behavior and
  PID shutdown, but not every rotation/race diagnostic.
- Obscure POSIX-version-dependent obsolete parsing modes are normalized to the
  GNU/MSYS behavior used by the local release oracle.

## sort

Updated:

- Rechecked GNU coreutils `src/sort.c`: ordering is driven by key fields,
  comparator flags, merge/check mode, and the active record delimiter.
- Fixed `sort -z` to use binary reads in both the general and simple lexical
  paths, preserving NUL-delimited records through binary-safe stdout writes.
- Aligned check-mode diagnostics to the GNU shape `FILE:LINE: disorder:
  RECORD`; `-C` remains quiet while preserving the nonzero exit status.
- Added direct regression coverage for NUL-delimited output and check-mode
  diagnostics. Added Release black-box probes for `sort -z`,
  `sort -t , -k 2,2n`, and `sort -c` stderr shape against GNU sort.

Remaining sort gaps versus GNU coreutils:

- External merge/spill-to-disk behavior, `--compress-program`, and true
  parallel merge execution are accepted for compatibility but do not yet
  reproduce GNU's full large-file merge machinery.
- Locale collation, full multibyte ordering, and exact GNU random-sort hashing
  semantics still need source-led parity work beyond the current hot probes.
- Debug-key diagnostics and obscure key modifier combinations need broader
  option-by-option probes before claiming complete GNU `sort` parity.

## cut

Updated:

- Rechecked GNU coreutils `src/cut.c`, especially the newer `-F LIST`
  shortcut and `--whitespace-delimited[=trimmed]` option path.
- Added `-F` as a fields-mode list option that implies whitespace-delimited
  input and a single-space default output delimiter. If `-d` is also
  supplied, the explicit delimiter follows GNU source behavior and takes over
  input splitting while the `-F` space output default remains.
- Added `--whitespace-delimited=trimmed` to ignore outer blank runs while
  preserving normal `-w` edge-empty-field behavior.
- Fixed `--output-delimiter=` to write a NUL output separator and made the
  cut output paths use binary-safe stdout writes for NUL-bearing records.
- Added direct regression coverage for `-F`, `-F` plus explicit `-d`,
  trimmed whitespace mode, empty output delimiters, and existing `-z`
  behavior. The existing Release black-box `cut -d , -f 2,4` probe still
  matches the locally available GNU/MSYS cut reference.

Remaining cut gaps versus GNU coreutils:

- The local GNU/MSYS reference is older and does not support `-F` or
  `--whitespace-delimited`, so those source-led options currently rely on
  direct tests rather than Release black-box oracle comparison.
- Full multibyte delimiter parsing follows the current UTF-8 helper path, not
  GNU's complete `mcel`/locale stack.
- The whole-file fallback remains simpler than GNU's block-streaming
  `cut_fields_bytesearch`/`cut_bytes` machinery for every option
  combination, although the common field path is streamed.

## expr

Updated:

- Reworked `expr` against GNU coreutils `src/expr.c` instead of the previous
  three-token numeric evaluator.
- Implemented the GNU precedence ladder: grouping, `+ TOKEN` quoting,
  `length`, `match`, `index`, `substr`, anchored `:`, arithmetic, comparisons,
  `&`, and `|`.
- Comparisons are numeric when both operands are integer-shaped, otherwise
  lexical, matching GNU's documented behavior.
- `|` and `&` short-circuit evaluation so unevaluated invalid arithmetic such
  as `expr 1 | 1 / 0` does not fail.
- `STRING : REGEXP` and `match STRING REGEXP` use `utils:regex` POSIX basic
  syntax, require a match at offset zero, return capture group 1 when present,
  and otherwise return matched logical length.
- Added unit coverage for precedence, grouping, zero/null exit status, boolean
  value propagation, short-circuiting, numeric-vs-lexical comparisons,
  keywords, anchored BRE capture, `match`, `+ TOKEN`, and invalid expression
  exit status.
- Added Release black-box probes against local GNU/MSYS `expr`: arithmetic
  precedence and anchored basic-regex capture both match stdout/exit status and
  run about `0.9x` of the reference runtime.

Remaining expr gaps versus GNU coreutils:

- Numeric arithmetic uses signed 64-bit integers, not GNU's GMP-backed
  arbitrary precision integers.
- String collation is byte/`std::string` lexical rather than full
  locale-sensitive `strcoll`.
- Logical character handling is UTF-8/Windows-wide-string based, not a full GNU
  multibyte locale stack.

## cmp

Updated:

- Reworked `cmp` against GNU diffutils' `src/cmp.c` structure: open both inputs
  in binary mode, seek/discard initial skip prefixes, compare fixed-size blocks
  with `memcmp`, and only scan inside a block after a difference is known.
- Added GNU-style quiet-mode regular-file size short-circuit: `cmp -s` returns
  different immediately when the remaining regular-file sizes differ and the
  requested byte limit would reach the shorter file's EOF.
- Added same-file/same-skip short-circuit after both inputs have been opened,
  matching GNU's "same file and same position is identical" behavior without
  hiding missing-file diagnostics.
- Removed the old UTF-8 BOM stripping behavior from the comparison path. GNU
  `cmp` is byte-oriented and does not treat BOMs as text metadata.
- Refreshed the black-box performance baseline: `cmp -s big.txt big-copy.txt`
  moved from about `8.0x` slower than the local GNU/MSYS reference to about
  `0.5x` of the reference runtime.

Remaining cmp gaps versus GNU diffutils:

- EOF diagnostics still follow the existing WinuxCmd/unit-test stdout shape in
  a few cases; GNU emits those diagnostics on stderr with byte/line suffixes.
- `-b/--print-bytes` prints octal byte values but does not yet include GNU's
  printable `cat -t` style character rendering.
- Count suffix parsing covers the current tested hot suffixes, not GNU's full
  suffix set through `P`, `E`, `Z`, and `Y`.

## cksum

Updated:

- Reworked POSIX CRC calculation against GNU coreutils'
  `src/cksum_crc.c`: CRC now streams input through a generated slice-by-8 table
  instead of reading the whole file into memory and updating one byte at a time.
- Kept the table generation clean-room: the polynomial and slice-by-8 update
  structure match the upstream algorithm, while the table values are generated
  locally at runtime startup from the polynomial.
- `cksum --check` now computes listed-file CRCs through the same streaming path
  and validates byte counts from the streaming accumulator.
- Refreshed the Release black-box performance baseline: `cksum big.txt` now
  runs about `0.8x` of the local GNU/MSYS reference runtime.

Remaining cksum gaps versus GNU coreutils:

- Hardware-accelerated CRC backends from GNU (`pclmul`, `avx2`, `avx512`,
  platform VMULL) are not implemented; the portable slice-by-8 path is the
  current default.
- `sysv` and `bsd` checksum modes still use the older in-memory code path.
- `--raw`/`--base64` are implemented for the current CRC-style digest shape,
  not the full GNU `cksum -a` multi-algorithm matrix.

## checksum family

Updated:

- Reworked the portable digest update paths after reading GNU coreutils'
  checksum wrapper model in `src/cksum.c` and the BLAKE2 stream/update layout
  in `src/blake2/b2sum.c` plus `src/blake2/blake2b-ref.c`.
- `b2sum` now uses WinuxCmd's portable BLAKE2b implementation end-to-end
  instead of the old SHA512/CNG fallback placeholder text and dependency.
- `sha224sum` and `sha384sum` now describe the real portable SHA-224/SHA-384
  implementations rather than the old CryptoAPI truncation fallback.
- The SHA-224/SHA-384/BLAKE2b update paths now compress full middle blocks
  directly from the stream buffer and only retain the tail block when required
  by the algorithm. This mirrors the upstream streaming structure without
  vendoring GPL sources.
- `scripts/benchmark-command-parity.py` now records build directory and CMake
  build type so Debug `/Od` baselines do not get mistaken for release
  performance.
- `scripts/audit-command-compatibility.py` now maps checksum, diffutils,
  less, and util-linux pager commands to local upstream source files.
- Refreshed the Release black-box performance baseline: `b2sum big.txt` is
  about `1.1x`, `sha224sum big.txt` about `1.3x`, `sha384sum big.txt` about
  `1.2x`, and no probe is `>=5x` slower than the GNU/MSYS reference.

Remaining checksum-family gaps versus GNU coreutils:

- GNU may use platform-specific optimized digest backends; WinuxCmd currently
  uses portable C++ digest code except where older command implementations
  still use Windows providers.
- `cksum -a` multi-algorithm raw/base64 coverage remains narrower than GNU's
  full matrix.

## rev

Updated:

- Reworked `rev` against util-linux `text-utils/rev.c` instead of the previous
  `std::getline` + `safePrintLn` implementation.
- Added `-0, --zero` to use NUL as the record separator.
- Records are processed as separator-delimited data: the body is reversed and
  the separator is written only when it existed in input.
- Final unterminated records stay unterminated; `rev` no longer appends a
  newline that was not present.
- Standard input is put into binary mode on Windows so CRLF and NUL input are
  visible to the command instead of being translated by the CRT.
- File operands are opened in binary mode, missing files report an error and
  processing continues with later operands, matching util-linux's multi-file
  failure shape.
- UTF-8 codepoint units are reversed as units for common valid UTF-8 input
  rather than blindly reversing every byte.
- Added unit coverage for stdin, unterminated final records, multiple files,
  missing-file continuation, NUL separators, CRLF-as-data behavior, UTF-8 unit
  reversal, and wildcard file operands.
- Added Release black-box probes against local util-linux/MSYS `rev`: large
  file reversal and NUL-separated records both match stdout/exit status; the
  large-file probe runs about `0.5x` of the reference runtime and `-0` about
  `1.0x`.

Remaining rev gaps versus util-linux:

- util-linux uses the active locale and `wchar_t` I/O for all records; WinuxCmd
  uses a UTF-8 unit parser with byte fallback for invalid sequences.
- Diagnostics follow the existing WinuxCmd style rather than util-linux
  `warn()` formatting byte-for-byte.

## cygpath

Updated:

- Reworked `cygpath` after reading Cygwin `winsup/utils/cygpath.cc` and
  black-box checking local MSYS/Git `cygpath.exe`.
- Corrected the default output mode to Unix form. Upstream sets `unix_flag`
  when no important output flag is provided; WinuxCmd now matches
  `cygpath C:\Users\Alice\Documents -> /c/Users/Alice/Documents`.
- Implemented hot output modes `-u/--unix`, `-w/--windows`, `-m/--mixed`, and
  `-t/--type` for `unix`, `windows`, and `mixed`.
- Implemented `-p/--path` path-list conversion in the upstream shape:
  Windows lists split on `;` and emit POSIX `:`, while POSIX lists split on
  `:` and emit Windows `;`.
- Added compatibility declarations for broader Cygwin options such as
  `--codepage`, `--long-name`, `--short-name`, system-folder outputs, and
  `--file`; unsupported ones are accepted by the option parser and then fail
  with an explicit diagnostic instead of silently succeeding.
- Added direct unit coverage for default conversion, explicit output modes,
  `--type`, path-list conversion, mixed path-list output, conflicting output
  modes, unsupported accepted options, and `--ignore`.
- Added Release black-box probes against local MSYS/Git `cygpath`: default
  Windows-to-Unix, `-p -u`, and `-p -m` all match stdout/exit status and run
  within reference runtime.

Remaining cygpath gaps versus Cygwin:

- The implementation does not call `cygwin_conv_path`/`cygwin_conv_path_list`;
  it clean-room mirrors common drive/UNC/path-list forms without requiring the
  Cygwin runtime.
- DOS short names, long-name/short-name normalization, system-folder outputs,
  codepage conversion, `--file`, and mode reporting remain explicit
  unsupported diagnostics.
- `/cygdrive` mount-prefix customization and `/proc/cygdrive` output are not
  implemented.

## test and [

Updated:

- Rechecked GNU coreutils `src/test.c`, especially the `[` main path and
  `posixtest` dispatch. Upstream strips the closing `]` first, then parses
  the expression tokens; operators like `-eq` are not command options.
- Fixed `[.exe` to evaluate `ctx.raw_args` instead of reconstructing the
  expression from generic option-parser positionals. This restores expressions
  such as `[ 12 -eq 13 ]` returning false (`1`) rather than parse-error (`2`).
- Kept the GNU-shaped closing-bracket error path: missing `]` exits with an
  error status.
- Added a direct `[` numeric false regression test and a Release black-box
  performance/parity probe for `[ 123 -gt 45 ]`.

Remaining test/[ gaps versus GNU coreutils:

- `test` and `[` still cover the current hot unary/binary forms, not the full
  GNU expression grammar with parenthesized precedence, `-l STRING`, `-ef`,
  `-nt`, `-ot`, and full locale collation.
- File predicates follow Win32 approximations for executability, ownership,
  special files, and symlinks.

## man

Updated:

- Pulled and reviewed man-db `src/man.c`. Upstream is a full manual-page
  locator/formatter/pager with `argp`, MANOPT parsing, manpath/locale
  resolution, formatter pipelines, and pager setup.
- Kept WinuxCmd `man` scoped as a built-in command manual browser rather than
  claiming man-db parity.
- Added direct unit coverage for `man --list`, `man ls`, missing command
  diagnostics, and no-argument usage output.
- Mapped `man` to local man-db upstream source in the generated compatibility
  audit so its scale gap is explicit.

Remaining man gaps versus man-db:

- No filesystem manpath database lookup, section selection, apropos/whatis,
  groff formatting, catman cache, MANOPT parsing, locale fallback, or external
  pager pipeline.
- Behavior is intentionally limited to `CommandRegistry` entries and should be
  documented as such until a full man-db-style workstream is planned.

## od

Updated:

- Reworked the common dump path after reading GNU coreutils `src/od.c`,
  especially `decode_format_string`, address-format selection, typed print
  functions, duplicate-block abbreviation, and final-address printing.
- Added GNU-shaped type handling for hot integer/character formats:
  `-a`, `-b`, `-c`, `-d`, `-o`, `-x`, `-t o|u|d|x|c|a[SIZE][z]`.
- Corrected string option parsing for `-A`, `-j`, `-N`, `-t`, and `-w`; the
  earlier implementation treated several string options as booleans.
- Implemented `-A n|o|d|x`, byte skipping, byte limits, optional `-w` width,
  little-endian partial-field padding, ASCII trailer output only for `z`, and
  binary stdin/file reads on Windows.
- Added direct unit coverage for `-An -tx1 -N16`, `-An -tx2` partial final
  fields, and `-An -tx1z`.
- Added a Release black-box probe against local GNU/MSYS `od` for
  `-An -tx1 -N16 big.txt`; stdout/exit status match and the current runtime is
  about `2.6x` of the reference, below the release `>=5x` concern threshold.

Remaining od gaps versus GNU coreutils:

- Floating-point formats, string-dump mode (`-S`), traditional offset grammar,
  all GNU size suffix combinations, locale-specific character rendering, and
  full old-style operand parsing still need source-led implementation.
- The implementation mirrors the common dump pipeline but is not yet scale- or
  option-equivalent to GNU's full 1.7k-line `od.c`.

## column

Updated:

- Reworked table tokenization after reading util-linux `text-utils/column.c`,
  especially the `greedy` flag, default tab/space separators, `-s` delimiter
  behavior, and default two-space output separator.
- Corrected `column -t` so default whitespace input collapses runs of spaces or
  tabs into fields instead of treating only tab as a delimiter.
- Corrected `column -t -s ,` so empty fields are preserved rather than silently
  dropped; this matches util-linux's non-greedy `strtok_r` path for supplied
  separators.
- Kept basic JSON output in the WinuxCmd shape while documenting that
  util-linux delegates JSON/table rendering to libsmartcols.
- Added direct unit coverage for exact table spacing, custom separators, empty
  fields, and CRLF record trimming.
- Added Release black-box probes against local util-linux/MSYS `column` for
  default whitespace tables and comma-separated empty fields; both match
  stdout/exit status and run within reference runtime.

Remaining column gaps versus util-linux:

- WinuxCmd does not embed libsmartcols. Advanced `--table-*` features such as
  per-column properties, tree output, wrapping/truncation/no-extreme logic,
  color schemes, terminal-width negotiation, and exact JSON shape remain scoped
  compatibility work.
- Width calculation is byte/string-width based, not full wide-character and
  ANSI-escape-aware table measurement.

## dd and pr probes

Updated:

- Added a Release black-box `dd` probe after reading GNU coreutils `src/dd.c`.
  The probe checks isolated filesystem state and stdout/exit status for
  `if=dd-input.bin of=dd-output.bin bs=4 count=3 status=none`; the current
  behavior matches the local GNU/MSYS reference and runs about `1.0x`.
- Added a Release black-box `pr` probe after reading GNU coreutils `src/pr.c`.
  The probe checks `pr -t -w 40 pr-basic.txt`, matching stdout/exit status and
  running about `0.9x`.

Remaining dd/pr gaps versus GNU coreutils:

- `dd` still needs option-by-option source-led coverage for conversions,
  direct/sparse/sync/noerror behavior, signal/status reporting, partial block
  accounting, and platform-specific truncation/fsync flags.
- `pr` still needs page-header, multi-column, merge, numbering, page-range,
  separator, form-feed, and tab-width parity tests. The current probe covers
  only the hot omit-header passthrough path.

## P1 Mode, Install, and Wrapper Utilities

Updated:

- Fixed `chmod` numeric mode handling after reading GNU coreutils `src/chmod.c`
  and black-boxing the MSYS-visible Windows attribute behavior. Owner write now
  drives ReadOnly: `444`/`u-w` become read-only, while `644`/`go-w` remain
  owner-writable.
- Fixed `install -m` after reading GNU coreutils `src/install.c`. `install`
  now parses numeric and common symbolic modes instead of checking whether the
  string contains `w`, applies the default owner-writable `0755` state after
  copying, and lets `-C` skip only when content and owner-write state match.
- Extended `vdir` to accept and forward GNU/`ls` format-related options such
  as `--format`, `--sort`, `--time`, `--color`, `--zero`, and
  `--group-directories-first`, matching the existing wrapper design over local
  `ls`.
- Implemented the hot GNU `df --output[=FIELD_LIST]` path after reading
  coreutils `src/df.c`: recognized fields are `source`, `fstype`, `size`,
  `used`, `avail`, `pcent`, `itotal`, `iused`, `iavail`, `ipcent`, `target`,
  and `file`; duplicate or unknown fields now fail instead of being silently
  ignored.
- Fixed `df -P` to use GNU's default `1024-blocks` header and changed
  `Capacity`/`Use%` to GNU's ceil-based `used / (used + available)` formula.
  Windows inode fields remain accepted in `--output` but print `-` because the
  Win32 volume APIs do not expose GNU-compatible inode accounting.
- Added direct unit coverage for `chmod 444`, `chmod go-w`, `install -m 644`,
  `install -m 444`, `install -C -m 444`, default `install` mode cleanup,
  `chgrp -R --dereference`, `chown -R --dereference`, and
  `vdir --format=single-column`, plus `df -P`, `df --output`, `df --output=`,
  and `df --output` mutual exclusions.
- Added Release black-box probes for `chmod`, `install`, `mktemp`, `dir`,
  `vdir`, `df`, `chgrp`, and `chown`. Mode-changing probes compare isolated
  filesystem state plus selected Windows ReadOnly/stat mode fingerprints;
  `chgrp`/`chown --reference` compare same-file ownership/group no-op paths;
  `mktemp -u` compares randomized stdout shape rather than exact random names;
  `df` compares stable GNU/MSYS output shapes for `-P` and custom `--output` fields.

Remaining P1 mode/wrapper gaps:

- `chmod` still does not implement the full gnulib mode grammar, ACL behavior,
  or all POSIX permission bits; Windows-visible parity is currently scoped to
  owner-write/ReadOnly behavior and recursive/reference handling.
- `install` still has Windows limitations for owner, group, SELinux context,
  and strip behavior. Those options remain accepted but documented as limited
  instead of silently promising full POSIX semantics.
- `dir`/`vdir` remain thin wrappers around WinuxCmd `ls`; broader `ls.c`
  option-order and default-format parity needs more source-led probes.
- `chgrp`/`chown` remain Windows ACL/SID-scoped rather than full POSIX UID/GID
  database and symlink-ownership implementations. No currently tracked P1
  mode/wrapper command lacks a Release probe.

## P1 Process Utilities

Updated:

- Reworked kill after reading GNU coreutils kill.c, procps-ng kill.c plus
  signals.c, and Cygwin kill.cc plus the Cygwin signal table.
- Added one command-owned signal parser for kill -lHUP, kill -l SIGHUP,
  kill -l1, kill -l 34, -s/-n, old-style -USR1, and numeric -NUM forms.
- Expanded kill -l and -L output to the Cygwin/MSYS Windows signal table,
  including aliases IOT, CLD, POLL, LOST and real-time conversions such as
  RTMIN+1 to 33 and 34 to RT2.
- Added direct tests for signal conversion, -0 existence probes, validated
  -q/--queue placeholder parsing, old-style signal names, and a safe
  controlled-child -9 termination E2E.
- Added two Release black-box probes against local Cygwin/MSYS kill.exe for
  kill -lHUP and kill -l34; both match stdout exactly.

Remaining kill gaps versus GNU/procps/Cygwin:

- Native Win32 does not expose POSIX queued signal payload delivery, so
  -q/--queue is accepted and integer-validated but remains a documented
  placeholder/no-op for delivery semantics.
- -f/--force and -W/--winpid are accepted Cygwin compatibility no-ops
  because WinuxCmd already operates on Windows PIDs through Win32 process
  handles.
- Nonzero POSIX signals cannot be delivered with true POSIX handler semantics
  to arbitrary native Windows processes; WinuxCmd maps them to process
  termination, with signal 0 reserved for existence checks.

## P1 Text/File Utilities

Updated:

- Fixed `nl` unnumbered-line formatting after reading GNU coreutils `src/nl.c`.
  WinuxCmd now emits a blank number field of `number-width + separator length`
  spaces for unnumbered lines instead of printing the separator itself.
- Added direct `nl` regression coverage for pattern-body numbering, blank-line
  grouping, and the blank number field behavior.
- Added Release black-box probes against local GNU/MSYS references for
  `nl -b p^ERR`, `tac -s :`, `stat -c %n:%s`, `split -l 1000`, and
  `csplit -s /^MARK/`. The `split` and `csplit` probes compare isolated
  filesystem state, not just stdout.
- Added a Release `realpath --relative-to` performance probe with stdout
  comparison disabled because WinuxCmd currently emits Windows backslashes
  where GNU/MSYS emits POSIX slashes.
- Added Release black-box probes for `join`, `shuf`, `timeout`, and `unlink`.
  `join` and deterministic `shuf --random-source` compare stdout exactly;
  `unlink` compares isolated filesystem state; `timeout` runs the same
  zero-output GNU `true` child via benchmark `{ref:true}` argument expansion.
- Added a Release `readlink -e` performance probe with stdout comparison
  disabled because canonicalized Windows paths can differ from GNU/MSYS path
  spelling even when they resolve to the same file.
- Hardened `readlink` unit tests so long-path and 8.3-short-path spellings of
  the same temporary directory compare as equivalent.
- Implemented source-led `strings` upgrades after reading GNU binutils
  `binutils/strings.c`: `-f/--print-file-name`,
  `-s/--output-separator`, `-w/--include-all-whitespace`, numeric `-MIN`,
  and `-e b/l/B/L` multi-byte scanning now have direct behavior instead of
  being parsed and ignored.
- Added direct `strings` coverage for filename prefixes, custom separators,
  default tab handling, `-w` newline handling, numeric minimum length, and
  UTF-16 big/little-endian extraction. Added a Release probe against GNU
  Binutils 2.40 for `strings -a -f -e l --output-separator=|`.

Remaining P1 text/file gaps versus GNU coreutils:

- `tac -r` still uses `utils:regex` Extended syntax, while GNU `tac.c` uses
  `re_compile_pattern` behavior. BRE-compatible patterns and literal
  separators are covered; ERE-only metacharacters such as `+` remain a known
  compatibility gap.
- `realpath --relative-to` and `readlink -e` path spellings differ on Windows.
  These are tracked as intentional platform output gaps until path-output
  normalization policy is settled for GNU-style commands.
- `strings -d/--data` and `-T/--target` remain documented placeholders because
  this build does not link BFD object-section parsing. `-U/--unicode` accepts
  GNU mode names and selects UTF-8 scanning for non-default modes, but full
  `locale`/`hex`/`escape`/`highlight` display transforms are not implemented.
- `split`, `csplit`, `stat`, `join`, `shuf`, `timeout`, `unlink`, `readlink`,
  and `strings` still need broader option-by-option parity probes beyond the
  current hot deterministic shapes.

## Other Regex Commands

Replaced `std::regex` in:

- `csplit`: pattern boundaries now use `utils:regex` Basic syntax.
- `find`: `-regex`/`-iregex` now use `utils:regex` full-path matching.
- `nl`: `pREGEXP` numbering styles now use `utils:regex` Basic syntax.
- `tac`: `-r` separator matching now uses `utils:regex` Extended syntax; GNU
  `tac.c` uses `re_compile_pattern`, so this remains a documented regex-syntax
  gap rather than a completed GNU-parity item.

Remaining `std::regex` use:

- None in `src/commands` or `src/utils`.

## more
Updated:
- Rechecked util-linux `text-utils/more.c`, especially `argscan`, which
  treats `-p/--clean-print` as a no-argument clean-print mode, accepts
  `-e/--exit-on-eof`, `-u/--plain`, long aliases for the hot pager flags,
  `-NUM` as a line-count shortcut, and `+NUM`/`+/pattern` as starting
  position directives before ordinary file operands.
- Corrected WinuxCmd `more -p file` and `more --clean-print file` so the
  file operand is not consumed as a fake pattern. `+/pattern` now owns the
  pattern-start behavior instead.
- Non-terminal output keeps the fast streaming path for plain passthrough, but
  switches to a line-processing path when `-s`, `+NUM`, or `+/pattern`
  needs observable filtering.
- Added direct unit coverage for non-tty passthrough, squeeze-blank behavior,
  clean-print short/long options, `-NUM`, `+NUM`, `+/pattern`, and
  accepted `--plain`/`--exit-on-eof` options.
- Added Release probes for `more -p more-small.txt` against `cat` and
  `more +3 more-small.txt` against `tail -n +3`; both pass and run below
  reference runtime in the current baseline.
Major gaps:
- Interactive util-linux commands such as numeric prefixes, `b`, `d`,
  `z`, search repeat, shell/editor escapes, and multi-file colon commands
  still need PTY-backed tests before claiming terminal parity.
- `-u/--plain` is accepted as a documented no-op because current output does
  not synthesize underline/bold overstrikes.
- Large filtered non-tty inputs use line buffering; plain passthrough remains
  streaming.

## less

Current `less` is still a simplified pager, but the first source-led terminal
navigation layer is now in place.

Updated:

- Rechecked upstream less `command.c`, `forwback.c`, and `pattern.c`.
  Upstream command dispatch maps forward/backward line and screen actions onto
  `forward()`/`backward()`, while search actions compile a remembered pattern
  and repeat it via `n`/reverse-repeat behavior.
- Refactored WinuxCmd `less` into a viewport model with a stable `top_line`
  instead of the previous forward-only prompt loop.
- Added common navigation keys: Space/PageDown, `b`/PageUp, Enter/Down,
  Up, `q`, and Esc.
- Added `/` forward search, `?` backward search, `n` repeat, and `N` reverse
  repeat. Search patterns compile inside `less` through `utils:regex` using
  POSIX Extended syntax, matching upstream less's GNU-regex
  `RE_SYNTAX_POSIX_EXTENDED` path.
- `-i` now follows less-style smart-case search, while `-I` forces
  case-insensitive search.
- Non-terminal output remains pass-through, matching the normal `less` shape
  for pipeline/file redirection and preserving the existing Release probe.

Major gaps:

- No search highlighting, search status diagnostics, or incremental search.
- No horizontal scroll/chop interaction beyond simple line truncation.
- Raw Windows Console input exists, but there is no full less-style alternate
  screen, terminal capability model, or PTY test harness yet.
- No prompt/status line parity.
- No multi-file navigation (`:n`, `:p`).
- No follow mode (`F`).
- Reads whole input into memory.

Recommended staged fix:

1. Move the console raw-mode/input-event helper into a reusable pager module.
2. Add PTY/manual harness coverage for navigation and search keys.
3. Add search highlighting, not-found diagnostics, and prompt parity.
4. Add horizontal scroll/home/end and mouse-wheel support.
5. Add multi-file and follow mode.
6. Move large inputs to a buffered line index instead of one full string.

## Release benchmark expansion
Updated:
- Rechecked upstream behavior against GNU coreutils sources for the base encoders and text-formatting tools: `../_upstream_refs/gnu-coreutils/src/basenc.c`, `../_upstream_refs/gnu-coreutils/src/expand.c`, `../_upstream_refs/gnu-coreutils/src/fold.c`, and `../_upstream_refs/gnu-coreutils/src/fmt.c`; for hexdump, rechecked util-linux `../_upstream_refs/util-linux/text-utils/hexdump.c`.
- Added Release black-box probes for `base64` large encode plus decode, `base32` large encode plus decode, `basenc --base64url` encode plus `--base16 -d`, `expand -t 4`, `unexpand -a -t 4`, `fold -s -w 20`, `fmt -w 40`, `hexdump -C -n 32`, and default `hexdump` compact two-byte hex.
- Refreshed `DOCS/generated/command_performance_baseline.*`: the current Release baseline has 195/195 passing probes and no tracked probe at or above 5x slower than reference.
- Refreshed `DOCS/generated/command_compatibility_matrix.*`: 131 commands now have at least one performance/parity probe recorded.
- Rechecked upstream behavior for the exact/state utility batch against GNU coreutils `factor.c`, `sum.c`, `numfmt.c`, `truncate.c`, `link.c`, `pathchk.c`, `tsort.c`, `printenv.c`, `true.c`, and checksum wrapper sources. Added Release probes for `sha1sum`, `sha256sum`, `sum -s`, `factor`, `numfmt --from=iec`, `pathchk -p`, `tsort`, `printenv LC_ALL`, `true`, `false`, `truncate -s`, and `link`.
- Rechecked GNU diffutils `sdiff.c` and added a scoped Release black-box probe for `sdiff -w 80`, using the GNU reference with `--diff-program` and matching stdout, stderr, and exit status for differing side-by-side output.
- Rechecked GNU diffutils `diff3.c`, replacing the local placeholder conflict summary with GNU-shaped default conflict blocks plus `-m` merge conflict markers for single contiguous overlapping changes. Added Release black-box probes for default and `-m` conflicts with the GNU reference pinned via `--diff-program`.
- Rechecked GNU sed `execute.c` range-state handling and added a Release black-box probe for `sed '0,/foo/s/foo/XX/'`, matching GNU sed stdout and exit status. Rechecked GNU findutils `pred.c`/`parser.c` predicate/action wiring and added Release probes for `find -empty -printf` plus `find -name dir_010 -prune -o ...`. Rechecked GNU coreutils `ls.c` recursive section handling and made the existing `ls -R tree` Release probe compare stdout exactly. Rechecked GNU coreutils `sort.c`, fixed NUL-delimited `sort -z` input/output, aligned check-mode diagnostics, and added Release probes for `sort -z`, field numeric keys, and `sort -c` stderr shape. Rechecked GNU coreutils `cut.c` and added source-led support for `-F`, `--whitespace-delimited=trimmed`, NUL output delimiters, and binary-safe cut output. Rechecked GNU coreutils `head.c`/`tail.c`, added `head` all-but-last seekable fast paths, fixed GNU `tail +NUM` obsolete parsing, and added head/tail Release probes for byte, line, and from-start hot paths. Rechecked GNU coreutils `cp.c`, `mv.c`, `rm.c`, and `remove.c`, fixed slash-style `cp --parents` path preservation, and added file-ops state probes for `cp`, `mv`, and `rm` hot paths. Rechecked GNU coreutils `tr.c`, `du.c`, `ln.c`, `mkdir.c`, and `rmdir.c`, fixed GNU-shaped `ln -v` and `mkdir -vp` output, and added `tr`, `du`, `ln`, `mkdir`, and `rmdir` Release probes. Rechecked GNU findutils `xargs.c` and GNU coreutils `uniq.c`, `comm.c`, and `paste.c`; added `comm` default-order diagnostics plus `uniq -d -u`, `uniq -D -u`, and `uniq -c -D` Release probes.
- Rechecked GNU coreutils `uname.c`, `nproc.c`, `tty.c`, `whoami.c`, `logname.c`, `hostid.c`, `sleep.c`, `sync.c`, and `groups.c` for the small system/identity utility batch. Fixed `nproc --ignore` integer option parsing and extra-operand rejection, then added Release probes for `arch`, `uname -m`, `nproc --ignore=1`, `tty`, `tty -s`, `whoami`, `logname`, `hostid` shape, `sleep 0`, and no-operand `sync`.
- Rechecked GNU coreutils `hostname.c`, `id.c`, and `nice.c`. Reworked `id` only-mode output and invalid option combinations to match GNU shape, including MSYS-style numeric current-user UID/GID mapping for `id -u` and `id -g`; added Release probes for `hostname`, `id -u`, `id -un`, `id -g`, and no-operand `nice`.
- Rechecked GNU coreutils `groups.c`, GNU coreutils `who.c`, GNU coreutils `users.c`, GNU coreutils `yes.c`, GNU coreutils `dircolors.c`, GNU which `which.c`, procps-ng `src/ps/parser.c`/`src/ps/output.c`, ncurses `progs/infocmp.c`, util-linux `text-utils/col.c`, Cygwin `utils/getconf.c`/`utils/locale.cc`, dos2unix `dos2unix.c`/`common.c`, and the upstream `file`/libmagic source tree. Added Release shape/performance probes for current `groups`, `who -q`, `users`, `ps`, `infocmp xterm`, `col -b`, `dircolors -b`, `getconf _NPROCESSORS_ONLN`, `locale`, `file big.txt`, `which true`, and a capped `yes alpha beta` prefix plus in-place state probes for `dos2unix`, `d2u`, `unix2dos`, and `u2d`. The `yes` fix keeps the GNU-style large repeated-write optimization while preserving complete repeated lines instead of cutting the repeat unit at an arbitrary byte boundary. The `col -b` fix now treats backspace as a column movement even when suppressing literal backspace output, matching util-linux overstrike behavior.
Major gaps:
- `hexdump -C -n 32` and default compact two-byte hex now exact-match util-linux for the binary fixture, including canonical 8-digit offsets, final offset lines, and skipped-base offsets; broader format-string coverage remains a follow-up.
- `base64`, `base32`, and `basenc` share GNU coreutils `basenc.c` in the mechanical upstream LOC mapping, so per-command behavior still needs command-specific probes rather than separate upstream files.
- `diff3` now has scoped default and `-m` overlapping-conflict parity coverage, but GNU ed-script modes and complex multi-block merge machinery still need source-led implementation work. `sdiff` now has scoped `-w 80` side-by-side parity coverage, but interactive `-o` and broader line-filter behavior still need GNU-led implementation work. `sort` now covers hot lexical, field-key, NUL-delimited, and check-mode shapes, while external merge and full locale/random semantics remain documented gaps. `cut` now covers GNU source-led `-F` and trimmed whitespace behavior, with black-box parity still scoped to options supported by the local GNU/MSYS reference. `head`/`tail` now cover common byte/line and from-start hot paths; full `tail -f` race/notification parity remains a documented Windows-specific gap. `tr`, `du`, `ln`, `mkdir`, and `rmdir` now have source-led scoped probes for hot paths, while locale, Unix metadata, and symlink privilege edge cases remain documented gaps. `xargs`, `uniq`, `comm`, and `paste` now cover the source-led hot pipeline semantics added in this batch; remaining gaps are process/TTY edge cases, locale collation, and streaming memory behavior. The small identity/system utility probes cover stable stdout/status shapes; `groups`, `who`, and `users` are shape-probed because Windows account/session data is machine dependent, while `sync` file modes and broader `sleep` interval/error cases still need scoped follow-up probes. `ps` now has a default process-list shape probe and currently sits below the 5x performance alert threshold, but broader procps option semantics remain a follow-up. The line-ending conversion aliases now have isolated filesystem-state parity probes for simple CRLF/LF conversion; Unicode/BOM, binary-file skipping, symlink, new-file mode, and metadata preservation remain scoped follow-ups. `id -G` still needs broader MSYS/Cygwin group-list SID mapping beyond the current exact UID/GID probes. `lsof -i TCP:80` works in the current build, but attached short-option argument form `lsof -iTCP:80` is still a parser compatibility gap.

## which
Updated:
- Rechecked GNU which upstream `../_upstream_refs/gnu-which/which.c` and manual notes in `../_upstream_refs/gnu-which/which.texi`, especially `find_command_in_path`, `path_search`, Windows `PATHEXT` handling, and the dot/tilde display branches.
- Implemented `-s/--skip-dot`, `--skip-tilde`, `--show-dot`, and `--show-tilde` instead of rejecting them as unsupported.
- Path lookup now mirrors GNU which's Windows stance by considering the current directory before PATH, expanding simple leading `~` PATH entries against HOME, accepting winuxsh/Cygwin-style drive paths such as `/c/Users/...` and `/cygdrive/c/...` in PATH/HOME, honoring PATHEXT with the GNU default order `.exe;.ps1;.bat;.cmd;.com`, and formatting found paths through normalized absolute output unless dot/tilde display options apply.
- Added direct unit coverage for current-directory skip/show behavior, HOME-result skip/show behavior, tilde PATH expansion, tilde PATH skipping, and winuxsh `/c/...` PATH/HOME inputs.
- Rechecked GNU cpio `src/main.c`/archive flow, procps-ng `free.c`/`top.c`/`watch.c`, lsof `lib/lsof.c`, upstream tree `tree.c`, GNU coreutils `uptime.c`, and man-db `src/man.c` for the remaining easy-to-isolate P2 command batch. Added local-only Release shape/performance probes for `cpio -o`, `free -m`, `lsof --no-headers -F -t 50`, `lsof -iTCP:80`, `man --list`, `top -b -n 1 --rows 8`, `tree -L 1 tree`, default/pretty/since `uptime`, finite `watch -n 0 -c 1 -t cmd /c echo watch-ok`, and finite failing-child `watch -n 0 -c 1 -t cmd /c exit 1`. These probes record Winux timing and output shape without pretending a stable GNU/MSYS reference exists for machine-dependent Windows state.
- Closed the issue #114 follow-up batch by fixing the source-led gaps that had stable non-interactive surfaces: `lsof -iTCP:80` now parses the attached internet filter, `top -b -n 1 --rows N` now exits through real `-n/--iterations` value parsing, `watch` now returns the finite child command status, and `dir`/`vdir` dispatch to in-process `ls` when the main dispatcher is available so dispatcher-based benchmarks do not depend on PATH command links.
- Refreshed `DOCS/generated/command_performance_baseline.*`: the current Release baseline has 260/260 passing probes, no probe failures, and no tracked probe at or above 5x slower than reference. Refreshed `DOCS/generated/command_compatibility_matrix.*`: every non-local command now has upstream source LOC mapping, unit-test count, and performance-probe count; `tzset` remains the only local-family command.
Major gaps:
- GNU which alias/function parsing options such as `--read-alias`, `--skip-alias`, `--read-functions`, `--skip-functions`, and `--tty-only` are not part of the current local option surface.
- Full `~user` expansion is intentionally left literal on Windows because there is no shell-style user database available in this command layer.
- A scoped GNU which black-box Release probe is now recorded for `which true`; alias/function options remain outside the local option surface.
