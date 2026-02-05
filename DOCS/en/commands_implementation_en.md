# Linux Command Implementation Plan

## Usage Methods

### Direct Usage (No Activation Required)

```bash
# Use winux prefix to access commands directly
winux ls -la
winux cat file.txt
winux cp source.txt dest.txt
```

### With Activation

```bash
# Activate for direct command access
winux activate

# Now use commands directly
ls -la
cat file.txt
cp source.txt dest.txt

# Deactivate when done
winux deactivate
```

### Help Information

```bash
# View available commands
winux list

# Check activation status
winux status

# View version information
winux version

# View main executable help
winuxcmd --help
```

## Description

This is the Linux command implementation plan document for the WinuxCmd project. We will implement Linux common commands one by one and record the implementation status.

## Implementation Priority

- 🔴 High Priority: Most commonly used commands
- 🟠 Medium Priority: Commonly used commands
- 🟢 Low Priority: Less commonly used commands

## Command List

### File Management Commands

| Command | Status | Priority | Parameter Options | Implementation Description |
|---------|--------|----------|-------------------|---------------------------|
| ls      | ✅ Implemented | 🔴 | -l, -a, -h, -r | List directory contents |
| cat     | ✅ Implemented | 🔴 | -n, -b, -s | Concatenate files and print to standard output |
| cp      | ✅ Implemented | 🔴 | -r, -i, -v | Copy files or directories |
| mv      | ✅ Implemented | 🔴 | -i, -v | Move or rename files or directories |
| rm      | ✅ Implemented | 🔴 | -r, -i, -f, -v | Delete files or directories |
| mkdir   | ✅ Implemented | 🔴 | -p, -v | Create directories |
| rmdir   | ⬜ Not Implemented | 🟠 | -p, -v | Remove empty directories |
| touch   | ⬜ Not Implemented | 🟠 | -a, -m, -t | Change file timestamps |
| chmod   | ⬜ Not Implemented | 🟠 | -R, -v | Change file permissions |
| chown   | ⬜ Not Implemented | 🟠 | -R, -v | Change file owner |

### Text Processing Commands

| Command | Status | Priority | Parameter Options | Implementation Description |
|---------|--------|----------|-------------------|---------------------------|
| grep    | ⬜ Not Implemented | 🔴 | -i, -r, -n, -v | Text search |
| sed     | ⬜ Not Implemented | 🟠 | -i, -n, -e | Stream editor |
| awk     | ⬜ Not Implemented | 🟠 | -F, -v | Text processing tool |
| sort    | ⬜ Not Implemented | 🟠 | -n, -r, -k, -u | Sort text |
| uniq    | ⬜ Not Implemented | 🟠 | -c, -d, -u | Remove duplicate lines |
| head    | ⬜ Not Implemented | 🟠 | -n | Display file head content |
| tail    | ⬜ Not Implemented | 🟠 | -n, -f | Display file tail content |
| cut     | ⬜ Not Implemented | 🟠 | -d, -f | Cut text |
| paste   | ⬜ Not Implemented | 🟢 | -d, -s | Merge files |
| tr      | ⬜ Not Implemented | 🟢 | -d, -s | Character translation |

### System Information Commands

| Command | Status | Priority | Parameter Options | Implementation Description |
|---------|--------|----------|-------------------|---------------------------|
| pwd     | ⬜ Not Implemented | 🔴 | | Display current working directory |
| who     | ⬜ Not Implemented | 🟠 | -a, -b, -r | Display current users |
| uname   | ⬜ Not Implemented | 🟠 | -a, -m, -n, -r, -s, -v | Display system information |
| top     | ⬜ Not Implemented | 🟠 | -d, -n | Display process information |
| ps      | ⬜ Not Implemented | 🟠 | -a, -u, -x, -e | Display process status |
| df      | ⬜ Not Implemented | 🟠 | -h, -i | Display disk space |
| du      | ⬜ Not Implemented | 🟠 | -h, -s | Display directory size |
| free    | ⬜ Not Implemented | 🟠 | -h | Display memory usage |
| uptime  | ⬜ Not Implemented | 🟢 | | Display system uptime |
| date    | ⬜ Not Implemented | 🟢 | -d, -s, +FORMAT | Display or set date and time |

### Network Commands

| Command | Status | Priority | Parameter Options | Implementation Description |
|---------|--------|----------|-------------------|---------------------------|
| ping    | ⬜ Not Implemented | 🔴 | -c, -i, -t | Test network connection |
| ifconfig| ⬜ Not Implemented | 🟠 | | Display network interface information |
| netstat | ⬜ Not Implemented | 🟠 | -a, -n, -t, -u | Display network status |
| curl    | ⬜ Not Implemented | 🟠 | -O, -L, -s, -v | Transfer data |
| wget    | ⬜ Not Implemented | 🟠 | -O, -c, -q | Download files |

### Other Common Commands

| Command | Status | Priority | Parameter Options | Implementation Description |
|---------|--------|----------|-------------------|---------------------------|
| echo    | ⬜ Not Implemented | 🔴 | -n | Display a line of text |
| printf  | ⬜ Not Implemented | 🟠 | FORMAT | Formatted output |
| find    | ⬜ Not Implemented | 🟠 | -name, -type, -size | Find files |
| which   | ⬜ Not Implemented | 🟠 | | Display command path |
| whereis | ⬜ Not Implemented | 🟢 | | Display file location |
| locate  | ⬜ Not Implemented | 🟢 | | Find files |
| man     | ⬜ Not Implemented | 🟢 | | View command manual |
| help    | ⬜ Not Implemented | 🟢 | | Display help information |

## Implementation Plan

1. First implement high-priority commands (ls, cat, cp, mv, rm, mkdir, ping, echo, pwd)
2. Then implement medium-priority commands
3. Finally implement low-priority commands
4. After each command is implemented, update the status and implementation description in the document

## Note on Help Command

The `help` command has been removed from the winux.ps1 script to avoid conflicts with the main executable's help system. Users should use `winuxcmd --help` for help information.

## Development Specifications

- Use C++23 standard implementation
- Follow Google C++ Style Guide
- Never use std::print/std::println for output
- Implement core functions and common parameters of commands
- Maintain compatibility with Linux command behavior
- Write detailed unit tests