# WinuxCmd Project TODO List

## Project Initialization (Phase 1)

### Documentation
- [ ] Create project README.md (English)
- [ ] Create Chinese documentation in DOCS/README_zh.md
- [ ] Create detailed command compatibility matrix (English and Chinese)
- [ ] Update Project_Rules.md according to new requirements

### Build System Setup
- [ ] Configure CMake build scripts for C++23 modules
- [ ] Set up dual-mode compilation targets (individual commands + combined mode)
- [ ] Configure size optimization flags
- [ ] Add support for individual commands and combined executable

### Core Module Foundation
- [ ] Refactor moxon_app to winuxcmd architecture
- [ ] Create core command processing module
- [ ] Implement path resolution (Linux to Windows path conversion)
- [ ] Create command parameter parsing tool
- [ ] Implement Linux-style formatted console output wrapper

## Command Implementation (Phase 2)

### File System Commands
- [ ] ls (list directory contents)
  - [ ] Basic functionality
  - [ ] -l (long format)
  - [ ] -a (show hidden files)
  - [ ] -h (human-readable sizes)
  - [ ] -r (reverse sort)
- [ ] pwd (print working directory)
- [ ] mkdir (create directory)
- [ ] rmdir (remove directory)
- [ ] rm (remove file)
  - [ ] -f (force)
  - [ ] -r (recursive)
- [ ] cp (copy file/directory)
- [ ] mv (move/rename file)
- [ ] touch (create empty file)

### Text Processing Commands
- [ ] cat (concatenate files)
- [ ] grep (search text)
  - [ ] Basic pattern matching
  - [ ] -i (ignore case)
  - [ ] -n (show line numbers)
  - [ ] -r (recursive)
- [ ] head (show first few lines)
- [ ] tail (show last few lines)
- [ ] sort (sort lines)
- [ ] uniq (report or omit duplicate lines)
- [ ] wc (word, line, character count)

### System Information Commands
- [ ] date (display current date/time)
- [ ] time (time command execution)

## Testing and Optimization (Phase 3)

### Compatibility Testing
- [ ] Test each command against Linux native output
- [ ] Verify path conversion edge cases
- [ ] Test parameter combinations
- [ ] Validate error message formatting

### Size Optimization
- [ ] Measure executable size for each command
- [ ] Optimize core modules to reduce size
- [ ] Implement dead code elimination
- [ ] Verify combined executable size ≤ 1MB

### Performance Testing
- [ ] Measure command execution speed
- [ ] Optimize slow operations
- [ ] Reduce memory usage

## Release Preparation (Phase 4)

### Final Testing
- [ ] Complete regression testing
- [ ] Test on multiple Windows versions (10/11)
- [ ] Validate Windows Defender compatibility

### Documentation Finalization
- [ ] Update all documentation
- [ ] Create user manual (English and Chinese)
- [ ] Generate command reference (English and Chinese)

### Packaging and Distribution
- [ ] Create release package structure
- [ ] Generate version number
- [ ] Create release notes
- [ ] Set up CI/CD pipeline

## Tracking

- Total commands to implement: at least 100
- Phase 1 completion date: TBD
- Phase 2 completion date: TBD
- Phase 3 completion date: TBD
- Phase 4 completion date: TBD

## Notes

- All implementations must follow Project_Rules.md
- Prioritize core parameters over rare parameters
- Focus on size optimization throughout development
- Maintain compatibility with Linux command behavior
- Use C++23 modules for all new code
- Separate interface (.cppm) and implementation (.cpp) files