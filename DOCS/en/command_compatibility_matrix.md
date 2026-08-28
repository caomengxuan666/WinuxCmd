# WinuxCmd Command Compatibility Matrix

> Auto-generated from source code analysis of `src/commands/*.cpp`  
> GNU coreutils reference: GNU coreutils 9.6  
> Generated: 2026-06-01

## Overview

| Metric | Count |
|--------|------:|
| Total commands | 169 |
| GNU Coreutils compatible | 59 |
| Coreutils (partial compat) | 14 |
| Non-Coreutils utilities | 96 |
| Total options defined | 1203 |
| GNU-compatible options | 735 |
| WinuxCmd extensions | 261 |
| Behavioral differences | 153 |
| Other (unclassified) | 54 |

### Status Legend

| Tag | Meaning |
|-----|---------|
| GNU | Fully compatible GNU coreutils option |
| EXT | WinuxCmd extension (not present in GNU coreutils) |
| DIFFERS | Option exists in GNU but behavior differs on Windows |
| NONE | Unclassified option (see individual command notes) |

---

## Coreutils Compatible (59 commands)

These commands follow GNU coreutils semantics with high option compatibility.

| Command | Options | GNU | EXT | DIFFERS | Description |
|---------|:-------:|:---:|:---:|:-------:|-------------|
| `basename` | 3 | 3 | 0 | 0 | Implementation for basename. |
| `cat` | 10 | 10 | 0 | 0 | Implemention for cat. |
| `comm` | 4 | 4 | 0 | 0 | Implementation for comm. |
| `cp` | 24 | 21 | 0 | 1 | Implemention for cp. |
| `csplit` | 3 | 3 | 0 | 0 | Implementation for csplit. |
| `cut` | 6 | 5 | 0 | 1 | Implemention for cut. |
| `df` | 11 | 8 | 0 | 1 | Implementation for df - display disk space usage |
| `dir` | 39 | 36 | 1 | 2 | Implementation for dir (ls with column default). |
| `dircolors` | 6 | 6 | 0 | 0 | Implementation for dircolors. |
| `du` | 13 | 13 | 0 | 0 | Implementation for du - estimate file space usage |
| `env` | 5 | 5 | 0 | 0 | Implemention for env. |
| `expand` | 0 | 0 | 0 | 0 | Implementation for expand. |
| `find` | 45 | 44 | 0 | 0 | Implementation for find command. |
| `fmt` | 2 | 2 | 0 | 0 | Implementation for fmt. |
| `fold` | 3 | 3 | 0 | 0 | Implementation for fold. |
| `getopt` | 8 | 8 | 0 | 0 | Implementation for getopt. |
| `grep` | 33 | 20 | 0 | 1 | Implemention for grep. |
| `head` | 1 | 1 | 0 | 0 |  |
| `id` | 6 | 5 | 0 | 0 | Implementation for id. |
| `install` | 6 | 4 | 0 | 2 | Implementation for install. |
| `join` | 6 | 6 | 0 | 0 | Implementation for join. |
| `ln` | 10 | 6 | 0 | 4 | Implementation for ln. |
| `ls` | 40 | 38 | 0 | 2 | Implemention for ls. |
| `mkdir` | 4 | 2 | 0 | 2 | Implemention for mkdir. |
| `mv` | 24 | 21 | 0 | 3 | Implemention for mv. |
| `nice` | 1 | 0 | 0 | 1 | Implementation for nice command. |
| `nproc` | 2 | 1 | 0 | 1 | Implementation for nproc command. |
| `numfmt` | 11 | 10 | 1 | 0 | Implementation for numfmt command. |
| `od` | 16 | 16 | 0 | 0 | Implementation for od command. |
| `paste` | 1 | 1 | 0 | 0 | Implementation for paste. |
| `pathchk` | 2 | 2 | 0 | 0 | Implementation for pathchk command. |
| `pr` | 17 | 17 | 0 | 0 | Implementation for pr. |
| `printenv` | 1 | 1 | 0 | 0 | Implementation for printenv command. |
| `ptx` | 7 | 7 | 0 | 0 |  |
| `rm` | 11 | 9 | 0 | 2 | Implemention for rm. |
| `rmdir` | 1 | 1 | 0 | 0 |  |
| `sed` | 13 | 11 | 0 | 2 | Basic sed implementation with s/// substitutions |
| `seq` | 16 | 15 | 1 | 0 | Implementation for seq. |
| `shred` | 5 | 5 | 0 | 0 |  |
| `shuf` | 4 | 4 | 0 | 0 | Implementation for shuf. |
| `sort` | 14 | 14 | 0 | 0 | Implementation for sort. |
| `split` | 3 | 3 | 0 | 0 | Implementation for split. |
| `stat` | 2 | 0 | 0 | 1 | Implementation for stat. |
| `stdbuf` | 0 | 0 | 0 | 0 | Implementation for stdbuf command. |
| `stty` | 2 | 0 | 0 | 2 | Implementation for stty. |
| `sum` | 0 | 0 | 0 | 0 | Implementation for sum. |
| `sync` | 0 | 0 | 0 | 0 | Implementation for sync command. |
| `tac` | 1 | 1 | 0 | 0 | Implementation for tac. |
| `tail` | 8 | 8 | 0 | 0 |  |
| `tee` | 3 | 3 | 0 | 0 | Implemention for tee. |
| `timeout` | 1 | 1 | 0 | 0 | Windows process timeout with signal and job control semantics. |
| `tr` | 2 | 1 | 1 | 0 | Implementation for tr. |
| `truncate` | 3 | 3 | 0 | 0 | Implementation for truncate. |
| `tty` | 1 | 1 | 0 | 0 | Implementation for tty command. |
| `unexpand` | 0 | 0 | 0 | 0 | Implementation for unexpand. |
| `uniq` | 5 | 5 | 0 | 0 | Implementation for uniq. |
| `vdir` | 39 | 37 | 1 | 1 | Implementation for vdir (ls -l equivalent). |
| `wc` | 6 | 6 | 0 | 0 |  |
| `who` | 14 | 7 | 0 | 7 | Implementation for who. |

## Coreutils Partial Compatibility (14 commands)

GNU coreutils commands with lower option coverage or significant behavioral differences.

| Command | Options | GNU | EXT | DIFFERS | Description |
|---------|:-------:|:---:|:---:|:-------:|-------------|
| `chcon` | 9 | 0 | 0 | 9 |  |
| `chgrp` | 10 | 0 | 0 | 10 | Implementation for chgrp. |
| `chmod` | 10 | 0 | 0 | 10 | Implemention for chmod. |
| `chown` | 12 | 0 | 0 | 12 | Implementation for chown command. |
| `chroot` | 3 | 0 | 0 | 3 |  |
| `date` | 8 | 2 | 0 | 6 | Implementation for date. |
| `echo` | 5 | 2 | 1 | 0 | Implementation for echo command. |
| `hostname` | 5 | 1 | 2 | 2 | Implementation for hostname. |
| `kill` | 82 | 1 | 79 | 2 | Implementation for kill. |
| `pinky` | 6 | 2 | 0 | 4 |  |
| `readlink` | 4 | 0 | 0 | 4 | Implementation for readlink. |
| `realpath` | 8 | 1 | 1 | 5 | Implementation for realpath - print the absolute path of files |
| `touch` | 4 | 0 | 0 | 4 |  |
| `uname` | 7 | 0 | 0 | 7 | Implementation for uname. |

## Non-Coreutils (96 commands)

WinuxCmd-specific utilities or Cygwin/MSYS2-origin tools not found in GNU coreutils.

| Command | Options | Description |
|---------|:-------:|-------------|
| `arch` | 1 | Implementation for arch. |
| `b2sum` | 3 | Implementation for b2sum. |
| `base32` | 1 | Implementation for base32 command (RFC 4648). |
| `base64` | 1 | Implementation for base64. |
| `basenc` | 9 | Implementation for basenc command (multiple encodings). |
| `cal` | 1 |  |
| `chattr` | 3 | Implementation for chattr. |
| `cksum` | 6 | Implementation for cksum with GNU parity options. |
| `clear` | 1 | Implementation for clear command. |
| `cmp` | 3 | Implementation for cmp. |
| `col` | 5 | Implementation for col. |
| `column` | 10 | Implementation for column. |
| `cpio` | 7 | Implementation for cpio command. |
| `cygpath` | 18 | Implementation for cygpath command. |
| `d2u` | 1 | Implementation for d2u command. |
| `dd` | 11 |  |
| `diff` | 30 | Implementation for diff. |
| `diff3` | 5 | Implementation for diff3 command. |
| `dirname` | 0 | Implementation for dirname. |
| `dos2unix` | 1 | Implementation for dos2unix command. |
| `envsubst` | 1 | Implementation for envsubst. |
| `expr` | 1 | Implementation for expr command. |
| `factor` | 2 | Implementation for factor command. |
| `false` | 1 | Implementation for false command. |
| `file` | 11 | Implementation for file - determine file type |
| `free` | 12 | Implementation for free. |
| `getconf` | 2 | Implementation for getconf command. |
| `getfacl` | 6 | Read Windows file ACLs in a stable text format. |
| `groups` | 1 | Implementation for groups. |
| `hexdump` | 12 | Implementation for hexdump. |
| `hmac256` | 1 |  |
| `hostid` | 1 | Implementation for hostid command. |
| `infocmp` | 1 |  |
| `killall` | 10 | Implementation for killall. |
| `ldd` | 5 | Print PE import dependencies. |
| `less` | 5 | Implementation for less. |
| `locale` | 4 | Implementation for locale command. |
| `logger` | 12 | Windows-native logger command. |
| `logname` | 1 | Implementation for logname command. |
| `lsattr` | 4 | Implementation for lsattr. |
| `lsof` | 8 | Implementation for lsof. |
| `man` | 3 | Implementation for man command. |
| `md5sum` | 3 | Implementation for md5sum. |
| `mkfifo` | 2 |  |
| `mkgroup` | 2 | Implementation for mkgroup. |
| `mknod` | 2 |  |
| `mkpasswd` | 4 | Implementation for mkpasswd. |
| `mktemp` | 2 | Implementation for mktemp. |
| `more` | 7 | Implementation for more. |
| `mpicalc` | 3 |  |
| `nl` | 2 | Implementation for nl. |
| `nohup` | 1 | Implementation for nohup command. |
| `patch` | 23 | Implementation for patch command. |
| `pgrep` | 6 | Implementation for pgrep. |
| `pidof` | 1 | Implementation for pidof. |
| `pkill` | 10 | Implementation for pkill. |
| `pldd` | 1 | Implementation for pldd. |
| `printf` | 1 | Implementation for printf command. |
| `ps` | 11 | Implementation for ps. |
| `pwd` | 1 | Implementation for pwd. |
| `regtool` | 1 | Small Windows-native registry tool. |
| `renice` | 4 | Implementation for renice. |
| `reset` | 12 |  |
| `rev` | 1 | Implementation for rev command. |
| `runcon` | 4 |  |
| `sdiff` | 8 | Implementation for sdiff command. |
| `sha1sum` | 3 | Implementation for sha1sum. |
| `sha224sum` | 3 | Implementation for sha224sum. |
| `sha256sum` | 3 | Implementation for sha256sum. |
| `sha384sum` | 3 | Implementation for sha384sum. |
| `sha512sum` | 3 | Implementation for sha512sum. |
| `sleep` | 1 | Implementation for sleep. |
| `strings` | 4 | Implementation for strings. |
| `test` | 39 | Implementation for test command. |
| `test_bracket` | 37 | Implementation for [ command (alias for test). |
| `tic` | 6 |  |
| `time` | 1 |  |
| `toe` | 7 |  |
| `top` | 9 | Implementation for top - display dynamic real-time information |
| `tput` | 1 |  |
| `tree` | 8 | Implementation for tree command. |
| `true` | 1 | Implementation for true command. |
| `tsort` | 1 |  |
| `tzset` | 1 |  |
| `u2d` | 1 | Implementation for u2d command. |
| `unix2dos` | 1 | Implementation for unix2dos command. |
| `unlink` | 1 | Implementation for unlink command. |
| `uptime` | 2 | Implementation for uptime. |
| `users` | 1 | Implementation for users. |
| `watch` | 2 | Implementation for watch. |
| `which` | 10 | Implemention for which. |
| `whoami` | 1 | Implementation for whoami. |
| `wpm` | 8 | Winux Package Manager internal command. |
| `xargs` | 6 | Implementation for xargs. |
| `xxd` | 8 |  |
| `yes` | 1 | Implementation for yes. |

---

## Detailed Option Reference

Option-by-option breakdown for the most complex commands.

### `kill` (82 options)

| Short | Long | Description | Status |
|-------|------|-------------|--------|
| `--s` | `----signal` | specify the signal to send | [DIFFERS] |
| `--n` | --- | specify the signal to send | [EXT] |
| `--L` | `----table` | list signal names in a table | [DIFFERS] |
| `--t` | --- | list signal names in a table | [EXT] |
| `--NUM` | --- | send signal number | [EXT] |
| `--0` | --- | check whether a process exists | [EXT] |
| `--9` | --- | send SIGKILL (force kill) | [EXT] |
| `--15` | --- | send SIGTERM (graceful termination) | [EXT] |
| `--HUP` | --- | send SIGHUP | [GNU] |
| `--SIGHUP` | --- | send SIGHUP | [EXT] |
| `--INT` | --- | send SIGINT | [EXT] |
| `--SIGINT` | --- | send SIGINT | [EXT] |
| `--QUIT` | --- | send SIGQUIT | [EXT] |
| `--SIGQUIT` | --- | send SIGQUIT | [EXT] |
| `--ILL` | --- | send SIGILL | [EXT] |
| `--SIGILL` | --- | send SIGILL | [EXT] |
| `--TRAP` | --- | send SIGTRAP | [EXT] |
| `--SIGTRAP` | --- | send SIGTRAP | [EXT] |
| `--ABRT` | --- | send SIGABRT | [EXT] |
| `--SIGABRT` | --- | send SIGABRT | [EXT] |
| `--IOT` | --- | send SIGIOT | [EXT] |
| `--SIGIOT` | --- | send SIGIOT | [EXT] |
| `--EMT` | --- | send SIGEMT | [EXT] |
| `--SIGEMT` | --- | send SIGEMT | [EXT] |
| `--FPE` | --- | send SIGFPE | [EXT] |
| `--SIGFPE` | --- | send SIGFPE | [EXT] |
| `--KILL` | --- | send SIGKILL | [EXT] |
| `--SIGKILL` | --- | send SIGKILL | [EXT] |
| `--BUS` | --- | send SIGBUS | [EXT] |
| `--SIGBUS` | --- | send SIGBUS | [EXT] |
| `--SEGV` | --- | send SIGSEGV | [EXT] |
| `--SIGSEGV` | --- | send SIGSEGV | [EXT] |
| `--SYS` | --- | send SIGSYS | [EXT] |
| `--SIGSYS` | --- | send SIGSYS | [EXT] |
| `--PIPE` | --- | send SIGPIPE | [EXT] |
| `--SIGPIPE` | --- | send SIGPIPE | [EXT] |
| `--ALRM` | --- | send SIGALRM | [EXT] |
| `--SIGALRM` | --- | send SIGALRM | [EXT] |
| `--TERM` | --- | send SIGTERM | [EXT] |
| `--SIGTERM` | --- | send SIGTERM | [EXT] |
| `--URG` | --- | send SIGURG | [EXT] |
| `--SIGURG` | --- | send SIGURG | [EXT] |
| `--STOP` | --- | send SIGSTOP | [EXT] |
| `--SIGSTOP` | --- | send SIGSTOP | [EXT] |
| `--TSTP` | --- | send SIGTSTP | [EXT] |
| `--SIGTSTP` | --- | send SIGTSTP | [EXT] |
| `--CONT` | --- | send SIGCONT | [EXT] |
| `--SIGCONT` | --- | send SIGCONT | [EXT] |
| `--CHLD` | --- | send SIGCHLD | [EXT] |
| `--SIGCHLD` | --- | send SIGCHLD | [EXT] |
| `--CLD` | --- | send SIGCLD | [EXT] |
| `--SIGCLD` | --- | send SIGCLD | [EXT] |
| `--TTIN` | --- | send SIGTTIN | [EXT] |
| `--SIGTTIN` | --- | send SIGTTIN | [EXT] |
| `--TTOU` | --- | send SIGTTOU | [EXT] |
| `--SIGTTOU` | --- | send SIGTTOU | [EXT] |
| `--IO` | --- | send SIGIO | [EXT] |
| `--SIGIO` | --- | send SIGIO | [EXT] |
| `--POLL` | --- | send SIGPOLL | [EXT] |
| `--SIGPOLL` | --- | send SIGPOLL | [EXT] |
| `--XCPU` | --- | send SIGXCPU | [EXT] |
| `--SIGXCPU` | --- | send SIGXCPU | [EXT] |
| `--XFSZ` | --- | send SIGXFSZ | [EXT] |
| `--SIGXFSZ` | --- | send SIGXFSZ | [EXT] |
| `--VTALRM` | --- | send SIGVTALRM | [EXT] |
| `--SIGVTALRM` | --- | send SIGVTALRM | [EXT] |
| `--PROF` | --- | send SIGPROF | [EXT] |
| `--SIGPROF` | --- | send SIGPROF | [EXT] |
| `--WINCH` | --- | send SIGWINCH | [EXT] |
| `--SIGWINCH` | --- | send SIGWINCH | [EXT] |
| `--PWR` | --- | send SIGPWR | [EXT] |
| `--SIGPWR` | --- | send SIGPWR | [EXT] |
| `--LOST` | --- | send SIGLOST | [EXT] |
| `--SIGLOST` | --- | send SIGLOST | [EXT] |
| `--USR1` | --- | send SIGUSR1 | [EXT] |
| `--SIGUSR1` | --- | send SIGUSR1 | [EXT] |
| `--USR2` | --- | send SIGUSR2 | [EXT] |
| `--SIGUSR2` | --- | send SIGUSR2 | [EXT] |
| `--RTMIN` | --- | send SIGRTMIN | [EXT] |
| `--SIGRTMIN` | --- | send SIGRTMIN | [EXT] |
| `--RTMAX` | --- | send SIGRTMAX | [EXT] |
| `--SIGRTMAX` | --- | send SIGRTMAX | [EXT] |

### `find` (45 options)

| Short | Long | Description | Status |
|-------|------|-------------|--------|
| `--path` | --- | file name matches shell pattern PATTERN | [GNU] |
| `--wholename` | --- | same as -path | [GNU] |
| `--nowarn` | --- | turn off file traversal warnings | [GNU] |
| `--warn` | --- | turn on file traversal warnings | --- |
| `--nouser` | --- | file has no user corresponding to its owner ID | [GNU] |
| `--readable` | --- | file can be read | [GNU] |
| `--writable` | --- | file can be written | [GNU] |
| `--executable` | --- | file can be executed or searched | [GNU] |
| `--inum` | --- | file has inode/file-index number N | [GNU] |
| `--links` | --- | file has N hard links | [GNU] |
| `--user` | --- | file is owned by user name or UID | [GNU] |
| `--group` | --- | file belongs to group name or GID | [GNU] |
| `--uid` | --- | file owner's numeric UID matches N | [GNU] |
| `--gid` | --- | file group's numeric GID matches N | [GNU] |
| `--size` | --- | file uses n units of space | [GNU] |
| `--amin` | --- | file was last accessed n minutes ago | [GNU] |
| `--atime` | --- | file was last accessed n*24 hours ago | [GNU] |
| `--print` | --- | print the full file name on the standard output | [GNU] |
| `--fprint` | --- | print the full file name into FILE | [GNU] |
| `--L` | --- | follow symbolic links | [GNU] |
| `--P` | --- | never follow symbolic links (default) | [GNU] |
| `--follow` | --- | dereference symbolic links | [GNU] |
| `--delete` | --- | delete files | [GNU] |
| `--exec` | --- | execute command | [GNU] |
| `--fprintf` | --- | print format into FILE | [GNU] |
| `--prune` | --- | prune tree | [GNU] |
| `--quit` | --- | exit immediately | [GNU] |
| `--true` | --- | always true | [GNU] |
| `--false` | --- | always false | [GNU] |
| `--regex` | --- | whole path matches regular expression | [GNU] |
| `--newerat` | --- | compare access time to literal time | [GNU] |
| `--newerBt` | --- | compare birth time to literal time | [GNU] |
| `--newerct` | --- | compare change time to literal time | [GNU] |
| `--newermt` | --- | compare modify time to literal time | [GNU] |
| `--d` | --- | same as -depth | [GNU] |
| `--daystart` | --- | measure times from start of today | [GNU] |
| `--mount` | --- | do not descend into other file systems | [GNU] |
| `--xdev` | --- | same as -mount | [GNU] |
| `--noleaf` | --- | do not optimize by assuming 2+ hard links | [GNU] |
| `-!` | --- | negate expression | [GNU] |
| `--not` | --- | negate expression | [GNU] |
| `--a` | --- | and expression | [GNU] |
| `--and` | --- | and expression | [GNU] |
| `--o` | --- | or expression | [GNU] |
| `--or` | --- | or expression | [GNU] |

### `ls` (40 options)

| Short | Long | Description | Status |
|-------|------|-------------|--------|
| `--a` | `----all` | do not ignore entries starting with . | [GNU] |
| `--A` | `----almost-all` | do not list implied . and .. | [GNU] |
| `--b` | `----escape` | print C-style escapes for nongraphic characters | [GNU] |
| `--C` | --- | list entries by columns | [GNU] |
| `--f` | --- | list all entries in directory order | [GNU] |
| `--F` | `----classify` | append indicator (one of */=>@\|) to entries | [GNU] |
| `--g` | --- | like -l, but do not list owner | [GNU] |
| `--i` | `----inode` | print the index number of each file | [GNU] |
| `--I` | `----ignore` | ignore entries matching PATTERN | [GNU] |
| `--l` | `----long-list` | use a long listing format | [GNU] |
| --- | `----long` | use a long listing format | [GNU] |
| `--m` | --- | fill width with a comma separated list of entries | [GNU] |
| `--N` | `----literal` | print entry names without quoting | [GNU] |
| `--o` | --- | like -l, but do not list group information | [GNU] |
| `--p` | --- | append / indicator to directories | [GNU] |
| `--Q` | `----quote-name` | enclose entry names in double quotes | [GNU] |
| `--r` | `----reverse` | reverse order while sorting | [GNU] |
| `--R` | `----recursive` | list subdirectories recursively | [GNU] |
| `--s` | `----size` | print the allocated size of each file, in blocks | [GNU] |
| `--S` | --- | sort by file size, largest first | [GNU] |
| `--t` | --- | sort by time, newest first | [GNU] |
| `--U` | --- | do not sort; list entries in directory order | [GNU] |
| `--v` | --- | natural sort of (version) numbers within text | [GNU] |
| `--x` | --- | list entries by lines instead of by columns | [GNU] |
| `--X` | --- | sort alphabetically by entry extension | [GNU] |
| `--Z` | `----context` | print any security context of each file | [GNU] |
| --- | `----sort` | sort entries by WORD | [GNU] |
| --- | `----format` | set output format | [GNU] |
| --- | `----time` | change time style | [GNU] |
| --- | `----block-size` | scale sizes by SIZE | [GNU] |
| --- | `----quoting-style` | use quoting style WORD | [GNU] |
| --- | `----indicator-style` | append indicator using WORD | [GNU] |
| --- | `----file-type` | append file type indicators, without * | [GNU] |
| `--1` | --- | list one file per line | [GNU] |
| `--G` | `----no-group` | in a long listing, don't print group names | [DIFFERS] |
| --- | `----group-directories-first` | group directories before files | [GNU] |
| --- | `----author` | show author in long format | [GNU] |
| --- | `----si` | like -h, but use powers of 1000 not 1024 | [DIFFERS] |
| --- | `----full-time` | like -l --time-style=full-iso | [GNU] |
| --- | `----zero` | end each output line with NUL instead of newline | [GNU] |

### `dir` (39 options)

| Short | Long | Description | Status |
|-------|------|-------------|--------|
| `--a` | `----all` | do not ignore entries starting with . | [GNU] |
| `--A` | `----almost-all` | do not list implied . and .. | [GNU] |
| `--b` | `----escape` | print C-style escapes for nongraphic characters | [GNU] |
| `--C` | --- | list entries by columns | [GNU] |
| `--F` | `----classify` | append indicator (one of */=>@\|) to entries | [GNU] |
| `--g` | --- | like -l, but do not list owner | [GNU] |
| `--i` | `----inode` | print the index number of each file | [GNU] |
| `--l` | --- | use a long listing format | [GNU] |
| --- | `----long` | use a long listing format | [GNU] |
| --- | `----long-list` | use a long listing format | [EXT] |
| `--m` | --- | fill width with a comma separated list of entries | [GNU] |
| `--o` | --- | like -l, but do not list group information | [GNU] |
| `--p` | --- | append / indicator to directories | [GNU] |
| `--Q` | `----quote-name` | enclose entry names in double quotes | [GNU] |
| `--r` | `----reverse` | reverse order while sorting | [GNU] |
| `--R` | `----recursive` | list subdirectories recursively | [GNU] |
| `--s` | `----size` | print the allocated size of each file, in blocks | [GNU] |
| `--S` | --- | sort by file size, largest first | [GNU] |
| `--t` | --- | sort by time, newest first | [GNU] |
| `--u` | --- | with -lt: sort by, and show, access time | [GNU] |
| `--U` | --- | do not sort; list entries in directory order | [GNU] |
| `--v` | --- | natural sort of (version) numbers within text | [GNU] |
| `--x` | --- | list entries by lines across | [GNU] |
| `--X` | --- | sort alphabetically by entry extension | [GNU] |
| `--1` | --- | list one file per line | [GNU] |
| --- | `----group-directories-first` | group directories before files | [DIFFERS] |
| --- | `----author` | show author in long format | [GNU] |
| --- | `----block-size` | scale sizes by SIZE | [GNU] |
| `--Z` | `----context` | print any security context of each file | [DIFFERS] |
| `--D` | `----dired` | generate output designed for Emacs dired mode | [GNU] |
| --- | `----file-type` | append file type indicators, without * | [GNU] |
| --- | `----indicator-style` | append indicator using WORD | [GNU] |
| `--N` | `----literal` | print entry names without quoting | [GNU] |
| `--G` | `----no-group` | in a long listing, don't print group names | [GNU] |
| --- | `----quoting-style` | use quoting style WORD | [GNU] |
| --- | `----si` | like -h, but use powers of 1000 not 1024 | [GNU] |
| --- | `----full-time` | like -l --time-style=full-iso | [GNU] |
| `--f` | --- | list all entries in directory order | [GNU] |
| --- | `----zero` | end each output line with NUL, not newline | [GNU] |

### `test` (39 options)

| Short | Long | Description | Status |
|-------|------|-------------|--------|
| `--n` | --- | string length is non-zero | [GNU] |
| `--z` | --- | string length is zero | [GNU] |
| `--b` | --- | file is block special | [GNU] |
| `--c` | --- | file is character special | [GNU] |
| `--d` | --- | file is a directory | [GNU] |
| `--e` | --- | file exists | [GNU] |
| `--f` | --- | file is a regular file | [GNU] |
| `--g` | --- | file has set-group-ID bit | [GNU] |
| `--G` | --- | file is owned by effective group ID | [GNU] |
| `--h` | --- | file is a symbolic link | [GNU] |
| `--L` | --- | file is a symbolic link | [GNU] |
| `--k` | --- | file has sticky bit | [GNU] |
| `--p` | --- | file is a named pipe | [GNU] |
| `--r` | --- | file is readable | [GNU] |
| `--s` | --- | file size is non-zero | [GNU] |
| `--S` | --- | file is a socket | [GNU] |
| `--t` | --- | file descriptor is a terminal | [GNU] |
| `--u` | --- | file has set-user-ID bit | [GNU] |
| `--w` | --- | file is writable | [GNU] |
| `--x` | --- | file is executable | [GNU] |
| `--O` | --- | file is owned by effective user ID | [GNU] |
| `--eq` | --- | integer equal | [GNU] |
| `--ne` | --- | integer not equal | [GNU] |
| `--lt` | --- | integer less than | [GNU] |
| `--le` | --- | integer less than or equal | [GNU] |
| `--gt` | --- | integer greater than | [GNU] |
| `--ge` | --- | integer greater than or equal | [GNU] |
| `--a` | --- | logical and | [GNU] |
| `--and` | --- | logical and | [EXT] |
| `--o` | --- | logical or | [GNU] |
| `--or` | --- | logical or | [EXT] |
| `-!` | --- | logical not | [GNU] |
| `-=` | --- | string equal | [GNU] |
| `-==` | --- | string equal | [GNU] |
| `-!=` | --- | string not equal | [GNU] |
| `-<` | --- | string less than | [GNU] |
| `-<=` | --- | string less than or equal | [GNU] |
| `->` | --- | string greater than | [GNU] |
| `->=` | --- | string greater than or equal | [GNU] |

### `vdir` (39 options)

| Short | Long | Description | Status |
|-------|------|-------------|--------|
| `--a` | `----all` | do not ignore entries starting with . | [GNU] |
| `--A` | `----almost-all` | do not list implied . and .. | [GNU] |
| `--b` | `----escape` | print C-style escapes for nongraphic characters | [GNU] |
| `--C` | --- | list entries by columns | [GNU] |
| `--F` | `----classify` | append indicator (one of */=>@\|) to entries | [GNU] |
| `--g` | --- | like -l, but do not list owner | [GNU] |
| `--i` | `----inode` | print the index number of each file | [GNU] |
| `--l` | --- | use a long listing format | [GNU] |
| --- | `----long` | use a long listing format | [GNU] |
| --- | `----long-list` | use a long listing format | [EXT] |
| `--m` | --- | fill width with a comma separated list of entries | [GNU] |
| `--o` | --- | like -l, but do not list group information | [GNU] |
| `--p` | --- | append / indicator to directories | [GNU] |
| `--Q` | `----quote-name` | enclose entry names in double quotes | [GNU] |
| `--r` | `----reverse` | reverse order while sorting | [GNU] |
| `--R` | `----recursive` | list subdirectories recursively | [GNU] |
| `--s` | `----size` | print the allocated size of each file, in blocks | [GNU] |
| `--S` | --- | sort by file size, largest first | [GNU] |
| `--t` | --- | sort by time, newest first | [GNU] |
| `--u` | --- | with -lt: sort by, and show, access time | [GNU] |
| `--U` | --- | do not sort; list entries in directory order | [GNU] |
| `--v` | --- | natural sort of (version) numbers within text | [GNU] |
| `--x` | --- | list entries by lines across | [GNU] |
| `--X` | --- | sort alphabetically by entry extension | [GNU] |
| `--1` | --- | list one file per line | [GNU] |
| --- | `----group-directories-first` | group directories before files | [GNU] |
| --- | `----author` | show author in long format | [GNU] |
| --- | `----block-size` | scale sizes by SIZE | [GNU] |
| `--Z` | `----context` | print any security context of each file | [DIFFERS] |
| `--D` | `----dired` | generate output designed for Emacs dired mode | [GNU] |
| --- | `----file-type` | append file type indicators, without * | [GNU] |
| --- | `----indicator-style` | append indicator using WORD | [GNU] |
| `--N` | `----literal` | print entry names without quoting | [GNU] |
| `--G` | `----no-group` | in a long listing, don't print group names | [GNU] |
| --- | `----quoting-style` | use quoting style WORD | [GNU] |
| --- | `----si` | like -h, but use powers of 1000 not 1024 | [GNU] |
| --- | `----full-time` | like -l --time-style=full-iso | [GNU] |
| `--f` | --- | list all entries in directory order | [GNU] |
| --- | `----zero` | end each output line with NUL, not newline | [GNU] |

### `test_bracket` (37 options)

| Short | Long | Description | Status |
|-------|------|-------------|--------|
| `--n` | --- | string length is non-zero | [GNU] |
| `--z` | --- | string length is zero | [GNU] |
| `--b` | --- | file is block special | [GNU] |
| `--c` | --- | file is character special | [GNU] |
| `--d` | --- | file is a directory | [GNU] |
| `--e` | --- | file exists | [GNU] |
| `--f` | --- | file is a regular file | [GNU] |
| `--g` | --- | file has set-group-ID bit | [GNU] |
| `--h` | --- | file is a symbolic link | [GNU] |
| `--l` | --- | file is a symbolic link | [GNU] |
| `--L` | --- | file is a symbolic link | [IMPL] |
| `--k` | --- | file has sticky bit | [GNU] |
| `--p` | --- | file is a named pipe | [GNU] |
| `--r` | --- | file is readable | [GNU] |
| `--s` | --- | file size is non-zero | [GNU] |
| `--t` | --- | file descriptor is a terminal | [GNU] |
| `--u` | --- | file has set-user-ID bit | [GNU] |
| `--w` | --- | file is writable | [GNU] |
| `--x` | --- | file is executable | [GNU] |
| `--eq` | --- | integer equal | [GNU] |
| `--ne` | --- | integer not equal | [GNU] |
| `--lt` | --- | integer less than | [GNU] |
| `--le` | --- | integer less than or equal | [GNU] |
| `--gt` | --- | integer greater than | [GNU] |
| `--ge` | --- | integer greater than or equal | [GNU] |
| `--a` | --- | logical and | [GNU] |
| `--and` | --- | logical and | [EXT] |
| `--o` | --- | logical or | [GNU] |
| `--or` | --- | logical or | [EXT] |
| `-!` | --- | logical not | [GNU] |
| `-=` | --- | string equal | [GNU] |
| `-==` | --- | string equal | [GNU] |
| `-!=` | --- | string not equal | [GNU] |
| `-<` | --- | string less than | [GNU] |
| `-<=` | --- | string less than or equal | [GNU] |
| `->` | --- | string greater than | [GNU] |
| `->=` | --- | string greater than or equal | [GNU] |

### `grep` (33 options)

| Short | Long | Description | Status |
|-------|------|-------------|--------|
| `--F` | `----fixed-strings` | PATTERNS are strings | [GNU] |
| `--G` | `----basic-regexp` | PATTERNS are basic regular expressions | [GNU] |
| `--P` | `----perl-regexp` | PATTERNS are Perl regular expressions | [GNU] |
| `--e` | `----regexp` | use PATTERNS for matching | [GNU] |
| `--f` | `----file` | take PATTERNS from FILE | --- |
| --- | `----no-ignore-case` | do not ignore case distinctions (default) | --- |
| `--w` | `----word-regexp` | match only whole words | [GNU] |
| `--x` | `----line-regexp` | match only whole lines | [GNU] |
| `--z` | `----null-data` | a data line ends in 0 byte, not newline | --- |
| `--s` | `----no-messages` | suppress error messages | [GNU] |
| `--v` | `----invert-match` | select non-matching lines | [GNU] |
| `--m` | `----max-count` | stop after NUM selected lines | [GNU] |
| `--b` | `----byte-offset` | print the byte offset with output lines | --- |
| `--n` | `----line-number` | print line number with output lines | [GNU] |
| --- | `----line-buffered` | flush output on every line | [GNU] |
| `--H` | `----with-filename` | print file name with output lines | --- |
| `--h` | `----no-filename` | suppress the file name prefix on output | --- |
| `--q` | `----quiet` | suppress all normal output | [GNU] |
| --- | `----silent` | suppress all normal output | --- |
| `--a` | `----text` | equivalent to --binary-files=text | [GNU] |
| `--I` | --- | equivalent to --binary-files=without-match | [GNU] |
| `--r` | `----recursive` | like --directories=recurse | [GNU] |
| `--R` | `----dereference-recursive` | like -r but follow symlinks | [GNU] |
| --- | `----include` | search only files that match GLOB | [GNU] |
| --- | `----exclude` | skip files that match GLOB | [GNU] |
| `--c` | `----count` | print only a count of selected lines per FILE | [GNU] |
| `--T` | `----initial-tab` | make tabs line up (if needed) | --- |
| `--Z` | `----null` | print 0 byte after FILE name | --- |
| `--C` | `----context` | print NUM lines of output context | [GNU] |
| `--NUM` | --- | same as --context=NUM | --- |
| --- | `----no-group-separator` | do not print group separator | --- |
| `--u` | `----unix-byte-offsets` | report Unix-style byte offsets | --- |
| `--U` | `----binary` | do not strip CR at EOL | [DIFFERS] |

### `diff` (30 options)

| Short | Long | Description | Status |
|-------|------|-------------|--------|
| `--q` | `----brief` | report only when files differ | [GNU] |
| `--U` | --- | output NUM lines of unified context | [GNU] |
| `--C` | --- | output NUM lines of copied context | [GNU] |
| --- | `----label` | use LABEL instead of file name | [GNU] |
| `--y` | `----side-by-side` | output in two columns | [GNU] |
| `--w` | `----ignore-all-space` | ignore all white space | [GNU] |
| `--a` | `----text` | treat all files as text | [GNU] |
| --- | `----binary` | read and write data as binary | [GNU] |
| --- | `----color` | colorize the output | [GNU] |
| `--d` | `----minimal` | try hard to find a smaller set of changes | [GNU] |
| --- | `----diff-program` | use PROGRAM to compare files | [GNU] |
| `--e` | `----ed` | output an ed script | [GNU] |
| --- | `----exclude` | exclude files that match PATTERN | [GNU] |
| `--f` | `----forward-ed` | output an ed script for current changes | [GNU] |
| `--i` | `----ignore-case` | ignore case when comparing files | [GNU] |
| --- | `----ignore-file-name-case` | ignore case for file names | [GNU] |
| `--l` | `----paginate` | pass output through pr | [GNU] |
| `--n` | `----rcs` | output an RCS-format diff | [GNU] |
| `--N` | `----new-file` | treat absent files as empty | [GNU] |
| --- | `----no-dereference` | don't follow symbolic links | [GNU] |
| `--r` | `----recursive` | recursively compare subdirectories | [GNU] |
| `--s` | --- | report when two files are the same | [GNU] |
| --- | `----suppress-blank-empty` | suppress empty common lines | [GNU] |
| `--t` | `----expand-tabs` | expand tabs to spaces in output | [GNU] |
| `--T` | `----initial-tab` | tab stop every NUM output lines | [GNU] |
| --- | `----tabsize` | tab stop every NUM print positions | [GNU] |
| `--W` | `----width` | output at most NUM columns | [GNU] |
| `--x` | --- | exclude files that match PAT | [GNU] |
| `--X` | --- | exclude files matching pattern in FILE | [GNU] |
| `--Z` | --- | strip trailing carriage return on input | [GNU] |

### `cp` (24 options)

| Short | Long | Description | Status |
|-------|------|-------------|--------|
| `--a` | `----archive` | same as -dR --preserve=all | [IMPL] |
| `--b` | --- | like --backup but does not accept an argument | [GNU] |
| `--d` | --- | same as --no-dereference --preserve=links | [GNU] |
| `--i` | `----interactive` | prompt before overwrite | [GNU] |
| `--H` | --- | follow command-line symbolic links in SOURCE | [GNU] |
| `--l` | `----link` | hard link files instead of copying | [GNU] |
| `--L` | `----dereference` | always follow symbolic links in SOURCE | [GNU] |
| `--P` | `----no-dereference` | never follow symbolic links in SOURCE | [GNU] |
| `--p` | --- | same as --preserve=mode,ownership,timestamps | [GNU] |
| `--R` | `----recursive` | copy directories recursively | [GNU] |
| `--r` | `----recursive` | copy directories recursively | [GNU] |
| `--s` | `----symbolic-link` | make symbolic links instead of copying | [GNU] |
| `--S` | `----suffix` | override the usual backup suffix | [GNU] |
| `--T` | `----no-target-directory` | treat DEST as a normal file | [GNU] |
| `--u` | `----update` | equivalent to --update[=older] | [GNU] |
| `--v` | `----verbose` | explain what is being done | [GNU] |
| --- | `----debug` | explain how a file is copied; implies --verbose | [GNU] |
| `--g` | `----progress-bar` | display a progress bar while copying | --- |
| `--x` | `----one-file-system` | stay on this file system | [GNU] |
| --- | `----parents` | use full source file name under DIRECTORY | [DIFFERS] |
| --- | `----parent` | use full source file name under DIRECTORY | [GNU] |
| --- | `----sparse` | control creation of sparse files | [GNU] |
| --- | `----reflink` | control clone/CoW copies | [GNU] |
| --- | `----preserve` | preserve the specified attributes | [GNU] |

### `mv` (24 options)

| Short | Long | Description | Status |
|-------|------|-------------|--------|
| `--b` | --- | like --backup but does not accept an argument | [GNU] |
| --- | `----debug` | explain how a file is moved | [GNU] |
| --- | `----exchange` | exchange source and destination | [DIFFERS] |
| `--f` | `----force` | do not prompt before overwriting | [GNU] |
| `--i` | --- | prompt before overwrite | [GNU] |
| `--I` | --- | prompt once before removing more than three files, or when moving recursively | [GNU] |
| `--n` | `----no-clobber` | do not overwrite an existing file | [GNU] |
| --- | `----no-copy` | do not copy if renaming fails | [GNU] |
| --- | `----strip-trailing-slashes` | remove any trailing slashes from each SOURCE argument | [GNU] |
| `--S` | `----suffix` | override the usual backup suffix | [GNU] |
| `--t` | `----target-directory` | move all SOURCE arguments into DIRECTORY | [GNU] |
| `--T` | `----no-target-directory` | treat DEST as a normal file | [GNU] |
| `--u` | --- | equivalent to --update[=older] | [GNU] |
| `--v` | `----verbose` | explain what is being done | [GNU] |
| `--Z` | `----context` | set SELinux security context of destination file to default type | [DIFFERS] |
| --- | `----backup` | make a backup of each existing destination file | [GNU] |
| --- | `----interactive` | prompt according to WHEN: never, once (-I), or always (-i) | [GNU] |
| --- | `----no-clobber` | do not overwrite an existing file | [GNU] |
| --- | `----suffix` | override the usual backup suffix | [GNU] |
| --- | `----target-directory` | move all SOURCE arguments into DIRECTORY | [GNU] |
| --- | `----no-target-directory` | treat DEST as a normal file | [GNU] |
| --- | `----update` | control which existing files are updated; UPDATE={all,none,older(default)} | [GNU] |
| --- | `----verbose` | explain what is being done | [GNU] |
| --- | `----context` | set SELinux security context of destination file to default type | [DIFFERS] |

### `patch` (23 options)

| Short | Long | Description | Status |
|-------|------|-------------|--------|
| `--p` | --- | strip NUM leading components from file names | [GNU] |
| `--i` | --- | read patch from FILE | [GNU] |
| `--b` | `----backup` | back up the original file | --- |
| --- | `----dry-run` | do not actually change any files | --- |
| `--d` | `----directory` | change to DIR first | --- |
| `--D` | `----ifdef` | use WORD to patch files | --- |
| --- | `----remove-empty-files` | remove empty output files | --- |
| `--f` | `----force` | force this patch, even if it seems reversed | --- |
| `--F` | `----fuzz` | set maximum fuzz factor | --- |
| `--l` | `----merge` | merge using merge program | --- |
| `--o` | `----output` | output to FILE instead of stdout | --- |
| --- | `----no-backup-if-mismatch` | don't backup if patch matches | --- |
| `--r` | `----reject-file` | output rejects to FILE | --- |
| --- | `----reject-format` | produce output in FORMAT | --- |
| `--s` | `----silent` | work silently unless an error occurs | --- |
| `--q` | `----quiet` | work silently unless an error occurs | --- |
| `--t` | `----batch` | same as --force, with diagnostics | --- |
| `--T` | `----set-time` | set the patch's modification time | --- |
| --- | `----set-utc` | set the patch's modification time to UTC | --- |
| `--u` | `----unified` | interpret the patch as unified diff | --- |
| `--v` | `----verbose` | print verbose output | --- |
| --- | `----binary` | read and write in binary mode | --- |
| --- | `----posix` | conform to POSIX standard | --- |

### `cygpath` (18 options)

| Short | Long | Description | Status |
|-------|------|-------------|--------|
| `--u` | `----unix` | print Unix form of NAME | [EXT] |
| `--w` | `----windows` | print Windows form of NAME | [EXT] |
| `--m` | `----mixed` | print Windows form, with regular slashes | [EXT] |
| `--p` | `----path` | NAME is a PATH list | [EXT] |
| `--d` | `----dos` | print DOS short form of NAMEs | [EXT] |
| `--i` | `----ignore` | ignore missing or empty arguments | [EXT] |
| `--f` | `----file` | read input paths from FILE | [EXT] |
| `--l` | `----long-name` | print Windows long form | [EXT] |
| `--s` | `----short-name` | print DOS short form | [EXT] |
| `--U` | `----proc-cygdrive` | emit /proc/cygdrive paths | [EXT] |
| `--M` | `----mode` | report file text/binary mode | [EXT] |
| `--A` | `----allusers` | use All Users for special folders | [EXT] |
| `--D` | `----desktop` | output Desktop directory | [EXT] |
| `--H` | `----homeroot` | output Profiles directory | [EXT] |
| `--O` | `----mydocs` | output My Documents directory | [EXT] |
| `--P` | `----smprograms` | output Start Menu Programs directory | [EXT] |
| `--S` | `----sysdir` | output system directory | [EXT] |
| `--W` | `----windir` | output Windows directory | [EXT] |

### `pr` (17 options)

| Short | Long | Description | Status |
|-------|------|-------------|--------|
| `--COLUMN` | --- | produce COLUMN-column output | [GNU] |
| `--a` | --- | produce multi-column output | [GNU] |
| `--d` | --- | double-space the output | [GNU] |
| `--e` | `----expand` | expand input TABs | [GNU] |
| `--h` | `----header` | use a centered HEADER | [GNU] |
| `--l` | `----length` | set page length | [GNU] |
| `--n` | `----number-lines` | number lines | [GNU] |
| `--o` | `----indent` | offset each line | [GNU] |
| `--s` | `----separator` | separate columns by characters | [GNU] |
| `--t` | `----omit-header` | omit page headers and trailers | [GNU] |
| `--w` | `----width` | set page width | [GNU] |
| `--W` | `----page-width` | set page width (default 72) | [GNU] |
| --- | `----across` | print across pages | [GNU] |
| --- | `----columns` | output COLUMN-column output | [GNU] |
| --- | `----double-space` | double-space the output | [GNU] |
| --- | `----expand-tabs` | expand input TABs | [GNU] |
| --- | `----pages` | begin printing with page PAGE | [GNU] |

### `od` (16 options)

| Short | Long | Description | Status |
|-------|------|-------------|--------|
| `--A` | `----address-radix` | select output address radix | [GNU] |
| `--a` | --- | select named character output | [GNU] |
| `--b` | --- | select octal byte output | [GNU] |
| `--c` | --- | select ASCII output | [GNU] |
| `--d` | --- | select unsigned decimal 2-byte output | [GNU] |
| `--j` | `----skip-bytes` | skip bytes | [GNU] |
| `--N` | `----read-bytes` | limit bytes | [GNU] |
| `--o` | --- | select octal 2-byte output | [GNU] |
| `--t` | `----format` | select output type | [GNU] |
| `--v` | `----output-duplicates` | write all input data | [GNU] |
| `--w` | `----width` | output bytes per line | [GNU] |
| `--x` | --- | select hexadecimal 2-byte units | [GNU] |
| `--f` | --- | select floating-point output | [GNU] |
| `--i` | --- | select signed decimal 2-byte output | [GNU] |
| `--l` | --- | select signed decimal 4-byte output | [GNU] |
| `--s` | --- | select signed decimal 2-byte output | [GNU] |

### `seq` (16 options)

| Short | Long | Description | Status |
|-------|------|-------------|--------|
| `--s` | `----separator` | use STRING to separate numbers | [GNU] |
| `--0` | --- |  | [GNU] |
| `--2` | --- |  | [GNU] |
| `--4` | --- |  | [GNU] |
| `--6` | --- |  | [GNU] |
| `--8` | --- |  | [GNU] |
| `--.` | --- |  | [GNU] |
| `---` | --- |  | [EXT] |
| `--A` | --- |  | [GNU] |
| `--E` | --- |  | [GNU] |
| `--i` | --- |  | [GNU] |
| `--n` | --- |  | [GNU] |
| `--inf` | --- |  | [GNU] |
| `--INF` | --- |  | [GNU] |
| `--Infinity` | --- |  | [GNU] |
| `--INFINITY` | --- |  | [GNU] |

### `sort` (14 options)

| Short | Long | Description | Status |
|-------|------|-------------|--------|
| `--b` | `----ignore-leading-blanks` | ignore leading blanks | [GNU] |
| `--f` | `----ignore-case` | fold lower case to upper case | [GNU] |
| `--i` | `----ignore-nonprinting` | consider only printable characters | [GNU] |
| `--M` | `----month-sort` | compare as month names | [GNU] |
| `--m` | `----merge` | merge already sorted files | [GNU] |
| `--V` | `----version-sort` | compare version numbers naturally | [GNU] |
| `--R` | `----random-sort` | sort by random hash of keys | [GNU] |
| --- | `----random-source` | get random bytes from FILE | [GNU] |
| `--r` | `----reverse` | reverse the result of comparisons | [GNU] |
| `--u` | `----unique` | output only the first of equal runs | [GNU] |
| `--z` | `----zero-terminated` | line delimiter is NUL, not newline | [GNU] |
| `--c` | `----check` | check whether input is sorted | [GNU] |
| `--C` | `----check-silent` | check whether input is sorted quietly | [GNU] |
| --- | `----debug` | print sort key diagnostics to standard error | [GNU] |

### `who` (14 options)

| Short | Long | Description | Status |
|-------|------|-------------|--------|
| `--a` | `----all` | same as -b -d --login -p -r -t -T -u | [DIFFERS] |
| `--b` | `----boot` | time of last system boot | [DIFFERS] |
| `--d` | `----dead` | print dead processes | [DIFFERS] |
| `--H` | `----heading` | print line of column headings | [GNU] |
| `--l` | `----login` | print system login processes | [DIFFERS] |
| `--m` | --- | only hostname and user associated with stdin | [GNU] |
| `--r` | `----runlevel` | print current runlevel | [DIFFERS] |
| `--s` | `----short` | print only name, line, and time | [GNU] |
| `--t` | `----time` | print last system clock change | [DIFFERS] |
| `--T` | --- | add user's message status as +, - or ? | [GNU] |
| `--u` | `----users` | list users logged in | [GNU] |
| `--w` | `----mesg` | add user's message status as +, - or ? | [GNU] |
| --- | `----message` | same as --mesg | [GNU] |
| --- | `----writable` | list only users with a writable terminal | [DIFFERS] |

### `du` (13 options)

| Short | Long | Description | Status |
|-------|------|-------------|--------|
| `--a` | `----all` | write counts for all files, not just directories | [GNU] |
| `--A` | `----apparent-size` | print apparent sizes | [GNU] |
| `--b` | `----bytes` | equivalent to '--apparent-size --block-size=1' | [GNU] |
| `--c` | `----total` | produce a grand total | [GNU] |
| --- | `----si` | print sizes in powers of 1000 (e.g., 1.1G) | [GNU] |
| `--k` | --- | like --block-size=1K | [GNU] |
| `--L` | `----dereference` | dereference all symbolic links | [GNU] |
| `--s` | `----summarize` | display only a total for each argument | [GNU] |
| --- | `----exclude` | exclude files that match PATTERN | [GNU] |
| --- | `----time-style` | show timestamps using STYLE | [GNU] |
| `--0` | `----null` | end each output line with NUL, not newline | [GNU] |
| `--l` | `----count-links` | count sizes many times if hard linked | [GNU] |
| `--m` | --- | like --block-size=1M | [GNU] |

### `sed` (13 options)

| Short | Long | Description | Status |
|-------|------|-------------|--------|
| `--n` | `----quiet` | suppress automatic printing of pattern space | [GNU] |
| --- | `----silent` | alias for -n | [GNU] |
| `--z` | `----null-data` | separate lines by NUL characters | [GNU] |
| --- | `----zero-terminated` | alias for -z | [GNU] |
| `--u` | `----unbuffered` | buffer input and output minimally | [GNU] |
| `--b` | `----binary` | open files in binary mode | [DIFFERS] |
| `--i` | `----in-place` | edit files in place | [GNU] |
| `--f` | `----file` | add the script from FILE | [GNU] |
| `--E` | `----regexp-extended` | use extended regular expressions | [GNU] |
| --- | `----debug` | annotate program execution (unsupported) | [DIFFERS] |
| --- | `----sandbox` | restrict file system access in the script | [GNU] |
| --- | `----posix` | disable GNU extensions and follow POSIX sed | [GNU] |
| `--r` | --- | alias for -E | [GNU] |

### `chown` (12 options)

| Short | Long | Description | Status |
|-------|------|-------------|--------|
| `--f` | `----silent` | suppress most error messages | [DIFFERS] |
| --- | `----quiet` | suppress most error messages | [DIFFERS] |
| --- | `----dereference` | affect referent of each symbolic link | [DIFFERS] |
| --- | `----from` | change only from current owner/group | [DIFFERS] |
| `--H` | --- | traverse command-line symlinks to directories | [DIFFERS] |
| `--L` | --- | traverse every symlink to a directory | [DIFFERS] |
| `--P` | --- | do not traverse any symbolic links | [DIFFERS] |
| `--R` | `----recursive` | operate on files and directories recursively | [DIFFERS] |
| --- | `----reference` | use RFILE's owner and group | [DIFFERS] |
| `--v` | `----verbose` | output a diagnostic for every file processed | [DIFFERS] |
| --- | `----preserve-root` | fail to operate recursively on '/' | [DIFFERS] |
| --- | `----no-preserve-root` | do not treat '/' specially | [DIFFERS] |

### `free` (12 options)

| Short | Long | Description | Status |
|-------|------|-------------|--------|
| `--b` | `----bytes` | display amount of memory in bytes | [GNU] |
| `--k` | `----kibi` | display amount of memory in kibibytes | [GNU] |
| `--m` | `----mebi` | display amount of memory in mebibytes | [GNU] |
| `--g` | `----gibi` | display amount of memory in gibibytes | [GNU] |
| `--h` | `----human` | show human-readable output | [GNU] |
| `--t` | `----total` | display a line showing the totals | [GNU] |
| `--c` | `----count` | repeat the display COUNT times | [GNU] |
| `--w` | `----wide` | wide output | [GNU] |
| --- | `----tebi` | display amount of memory in tebibytes | [GNU] |
| --- | `----pebi` | display amount of memory in pebibytes | [GNU] |
| --- | `----si` | use powers of 1000 not 1024 | [GNU] |
| `--L` | `----line` | show output on a single line | [GNU] |

### `hexdump` (12 options)

| Short | Long | Description | Status |
|-------|------|-------------|--------|
| `--b` | --- | one-byte octal display | [GNU] |
| `--c` | --- | one-byte character display | [GNU] |
| `--C` | --- | canonical hex+ASCII display | [GNU] |
| `--d` | --- | two-byte decimal display | [GNU] |
| `--o` | --- | two-byte octal display | [GNU] |
| `--x` | --- | two-byte hex display | [GNU] |
| `--e` | --- | format string | [GNU] |
| `--f` | --- | format file | [GNU] |
| `--n` | --- | interpret only LENGTH bytes of input | [GNU] |
| `--s` | --- | skip offset bytes from the beginning | [GNU] |
| `--v` | --- | display all input, no squeeze | [GNU] |
| `--L` | `----color` | interpret color formatting specifiers | [GNU] |

### `logger` (12 options)

| Short | Long | Description | Status |
|-------|------|-------------|--------|
| `--i` | `----id` | log the logger command's PID | [GNU] |
| `--f` | `----file` | log the contents of this file | [GNU] |
| `--p` | `----priority` | message priority | [GNU] |
| `--s` | `----stderr` | also write the message to standard error | [GNU] |
| `--t` | `----tag` | mark every line with this tag | [GNU] |
| `--n` | `----server` | write to this remote syslog server | [GNU] |
| `--T` | `----tcp` | use TCP only | [GNU] |
| `--d` | `----udp` | use UDP only | [GNU] |
| --- | `----no-act` | do everything except the write the log | [GNU] |
| `--S` | `----size` | maximum size for a single message | [GNU] |
| --- | `----rfc3164` | use the obsolete BSD syslog protocol | [GNU] |
| --- | `----rfc5424` | use the syslog protocol (default for remote) | [GNU] |

### `reset` (12 options)

| Short | Long | Description | Status |
|-------|------|-------------|--------|
| `--I` | --- | no initialization strings | [EXT] |
| `--c` | --- | set control characters | [EXT] |
| `--e` | --- | erase character | [DIFFERS] |
| `--i` | --- | interrupt character | [DIFFERS] |
| `--k` | --- | kill character | [DIFFERS] |
| `--m` | --- | map identifier to type | [DIFFERS] |
| `--q` | --- | display term only, do no changes | [EXT] |
| `--Q` | --- | do not output control key settings | [EXT] |
| `--r` | --- | display term on stderr | [EXT] |
| `--s` | --- | output TERM set command | [EXT] |
| `--V` | --- | print curses-version | [EXT] |
| `--w` | --- | set window-size | [DIFFERS] |

---

## Known Limitations

### Windows File System
- Case-insensitive filenames by default (configurable per volume)
- No native symlinks for unprivileged users (requires Developer Mode or elevated)
- No Unix permission model; POSIX modes mapped via Windows ACLs
- Path separators differ: backslash (`\\`) vs. forward slash (`/`)
- Drive letters (`C:`, `D:`) do not exist in POSIX
- File locking semantics differ (mandatory vs. advisory)

### SELinux / Security Contexts
- `-Z` / `--context` options are accepted as no-ops on Windows
- `chcon` and `runcon` parse arguments but cannot modify Windows security descriptors
- `getfacl` operates on Windows ACLs, not POSIX ACLs

### Process Management
- `kill`, `killall`, `pgrep`, `pkill` use Windows PIDs and signal mapping
- Unix signals mapped to Windows exceptions (SIGTERM -> TerminateProcess)
- `nice` / `renice` use Windows priority classes, not nice values
- `timeout` uses Windows job objects for process termination

### Terminal / TTY
- `stty` options mapped to Windows console API
- `tput` uses Windows terminfo database
- `clear` uses Windows console escape sequences
- `reset` behavior may differ from Linux terminal reset

### File Utilities
- `ln -s` creates Windows symlinks (requires appropriate privileges)
- `chattr` / `lsattr` mapped to NTFS file attributes
- `df` reports Windows drive volumes, not mount points
- `du` counts NTFS allocation units

### Data Processing
- `sort` uses locale-aware comparison; default may differ from C locale
- `sed` extended regex may have minor differences from GNU sed
- `grep` PCRE support depends on build configuration

---

## Windows-Specific Behavior

1. **Path Handling**: All commands accept both `/` and `\\` as path separators. Drive letters are preserved in output.

2. **Line Endings**: Input/output defaults to LF (Unix-style) but CRLF is detected and handled transparently.

3. **Executable Extensions**: Commands resolve `.exe`, `.bat`, `.cmd`, `.ps1` extensions when searching PATH.

4. **Environment Variables**: Windows-style `%VAR%` and Unix-style `$VAR` are both supported.

5. **Cygwin/MSYS Compatibility**: Many commands include Cygwin/MSYS2 path translation (via `cygpath`).

6. **Line Conversion**: `dos2unix`, `unix2dos`, `d2u`, `u2d` for native line-ending conversion.

7. **Registry Access**: `regtool` provides Windows registry manipulation (no Unix equivalent).

8. **Package Management**: `wpm` integrates with Windows Package Manager (no Unix equivalent).

---

## Build Information

| Property | Value |
|----------|-------|
| Compiler | MSVC (Visual Studio) |
| Build System | CMake + Visual Studio solution |
| Target Platform | Windows 10/11 (x64) |
| C++ Standard | C++23 modules |
| License | MIT |
| Repository | `unixwin/winuxcmd` |

---

*Generated from source code analysis of `src/commands/*.cpp`. For the latest information, consult the source repository.*
