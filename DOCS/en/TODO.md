# WinuxCmd Project TODO List

## Completed Items (Phase 1 - Done)

### Documentation ✅
- [x] Create project README.md (English)
- [x] Create Chinese documentation in DOCS/README_zh.md
- [x] Create detailed command compatibility matrix (English and Chinese)
- [x] Update Project_Rules.md according to new requirements

### Build System Setup ✅
- [x] Configure CMake build scripts for C++23 modules
- [x] Set up dual-mode compilation targets (individual commands + combined mode)
- [x] Configure size optimization flags
- [x] Add support for individual commands and combined executable
- [x] Implement build modes (DEV/RELEASE/DEBUG_RELEASE)
- [x] Add CMakePresets.json for easy configuration

### Core Module Foundation ✅
- [x] Create core command processing module
- [x] Implement path resolution (Linux to Windows path conversion)
- [x] Create command parameter parsing tool
- [x] Implement Linux-style formatted console output wrapper
- [x] Add color output support
- [x] Implement pipeline-based architecture

## Command Implementation Status (Phase 2)

### File System Commands ✅
- [x] ls (list directory contents)
  - [x] Basic functionality
  - [x] -l (long format)
  - [x] -a (show hidden files)
  - [x] -h (human-readable sizes)
  - [x] -r (reverse sort)
  - [x] -t (sort by time)
  - [x] --color (color output)
- [x] mkdir (create directory)
  - [x] -p (parents)
  - [x] -v (verbose)
  - [x] -m (mode)
- [x] rmdir (remove directory)
  - [x] --ignore-fail-on-non-empty
  - [x] -p (parents)
  - [x] -v (verbose)
- [x] rm (remove file)
  - [x] -f (force)
  - [x] -i (interactive)
  - [x] -r (recursive)
  - [x] -v (verbose)
- [x] cp (copy file/directory)
  - [x] -i (interactive)
  - [x] -r (recursive)
  - [x] -v (verbose)
  - [x] -f (force)
- [x] mv (move/rename file)
  - [x] -i (interactive)
  - [x] -v (verbose)
  - [x] -f (force)
  - [x] -n (no-clobber)
- [x] touch (create empty file)
  - [x] -a (access time)
  - [x] -m (modification time)
  - [x] -c (no-create)
  - [x] -d (date)
  - [x] -r (reference)

### Text Processing Commands ✅
- [x] cat (concatenate files)
  - [x] -n (number lines)
  - [x] -b (number non-blank)
  - [x] -E (show ends)
  - [x] -s (squeeze blank)
- [x] grep (search text)
  - [x] Basic pattern matching
  - [x] -E (extended regex)
  - [x] -F (fixed strings)
  - [x] -G (basic regex)
  - [x] -i (ignore case)
  - [x] -n (show line numbers)
  - [x] -v (invert match)
  - [x] -c (count)
  - [x] -l (files with matches)
  - [x] -L (files without match)
  - [x] --color (highlight matches)
- [x] head (show first few lines)
  - [x] -n (lines)
  - [x] -c (bytes)
  - [x] -q (quiet)
  - [x] -v (verbose)
- [x] tail (show last few lines)
  - [x] -n (lines)
  - [x] -c (bytes)
  - [x] -z (zero terminated)
  - [x] -f (follow)
- [x] sort (sort lines)
  - [x] -b (ignore blanks)
  - [x] -f (ignore case)
  - [x] -n (numeric)
  - [x] -r (reverse)
  - [x] -u (unique)
  - [x] -k (key)
  - [x] -t (field separator)
- [x] uniq (report or omit duplicate lines)
  - [x] -c (count)
  - [x] -d (duplicates)
  - [x] -f (skip fields)
  - [x] -i (ignore case)
  - [x] -s (skip chars)
  - [x] -u (unique)
  - [x] -w (check chars)
- [x] wc (word, line, character count)
  - [x] -c (bytes)
  - [x] -l (lines)
  - [x] -w (words)
  - [x] -m (chars)
  - [x] -L (max line length)
- [x] cut (cut fields from lines)
  - [x] -d (delimiter)
  - [x] -f (fields)
  - [x] -s (only delimited)
  - [x] -z (zero terminated)
- [x] sed (stream editor)
  - [x] -e (expression)
  - [x] -f (file)
  - [x] -n (quiet)
  - [x] -i (in-place)

### System Information Commands ✅
- [x] which (locate commands in PATH)
  - [x] -a (all matches)
  - [x] --skip-dot
  - [x] --skip-tilde
- [x] env (environment variables)
  - [x] -i (ignore environment)
  - [x] -u (unset)
  - [x] -0 (null separator)

## Future Enhancements (Phase 3 - Planned)

### Advanced Commands
- [ ] awk (pattern scanning and processing language)
- [ ] xargs (build and execute command lines from standard input)
- [ ] tee (read from standard input and write to standard output and files)
- [ ] find advanced features (-exec, -ok, -printf)
- [ ] date advanced formatting
- [ ] time (measure command execution time)

### Network Commands
- [ ] ping (send ICMP ECHO_REQUEST to network hosts)
- [ ] curl (transfer data from or to a server)
- [ ] wget (non-interactive network downloader)

### Archive Commands
- [ ] tar (tape archiver)
- [ ] gzip/gunzip (compression/decompression)
- [ ] zip/unzip (ZIP archive manipulation)

### System Commands
- [ ] ps (report process status)
- [ ] kill (send signal to processes)
- [ ] df (report file system disk space usage)
- [ ] du (estimate file space usage)

## Testing and Optimization (Ongoing)

### Compatibility Testing
- [ ] Test each command against Linux native output
- [ ] Verify path conversion edge cases
- [ ] Test parameter combinations
- [ ] Validate error message formatting

### Performance Optimization
- [ ] Profile command execution for bottlenecks
- [ ] Optimize memory usage patterns
- [ ] Improve startup time
- [ ] Reduce I/O overhead

### Build System Improvements
- [ ] Add more build presets
- [ ] Implement cross-compilation support
- [ ] Add static analysis integration
- [ ] Improve CI/CD pipeline

## Documentation Updates
- [ ] Create comprehensive user guide
- [ ] Add advanced usage examples
- [ ] Document internal architecture
- [ ] Create contribution guidelines

## Tracking

- Total commands implemented: 20+
- Phase 1 completion date: 2026
- Phase 2 completion date: 2026
- Phase 3 target date: TBD

## Notes

- All implementations must follow Project_Rules.md
- Prioritize core parameters over rare parameters
- Focus on performance and compatibility throughout development
- Maintain clean, well-documented code
- Use C++23 modules for all new code
- Separate interface (.cppm) and implementation (.cpp) files

