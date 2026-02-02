#!/bin/bash
# test_ls_bash.sh - Comprehensive ls command test (bash version)

set -e  # Exit on error

OUTPUT_FILE="ls_bash_results.txt"
echo "=== Linux ls Test Results ===" > "$OUTPUT_FILE"
echo "Test started: $(date)" >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

# Create test directory
TEST_DIR="ls_test_$(date +%s)"
rm -rf "$TEST_DIR"
mkdir -p "$TEST_DIR"
cd "$TEST_DIR" || exit 1

echo "Test directory: $(pwd)" >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

# Create test file structure
echo "Creating test files..."

# Create various files
touch normal.txt .hidden.txt "file with spaces.txt" backup.txt~ executable.sh UPPERCASE.TXT lowercase.txt MixedCase.txt
chmod +x executable.sh

# Create directory structure
mkdir -p empty_dir files_dir deep/level1/level2

# Create files in subdirectories
touch files_dir/file1.txt files_dir/file2.txt files_dir/file3.txt
touch deep/level1/level2/deepfile.txt

# Create files of different sizes
echo "Small file" > normal.txt
echo "Hidden content" > .hidden.txt
echo "Backup content" > backup.txt~
echo '#!/bin/bash' > executable.sh
echo 'echo "Executable"' >> executable.sh

# Create large file (10KB)
dd if=/dev/zero of=large.dat bs=1K count=10 2>/dev/null

# Create file with spaces
echo "File with spaces" > "file with spaces.txt"

# Set different modification times
touch -d "30 days ago" file1.txt
touch -d "15 days ago" file2.txt
touch -d "7 days ago" file3.txt

# Show created files
echo "Created files:" >> "$OUTPUT_FILE"
ls -la >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

# Test counters
TEST_COUNT=0
ERROR_COUNT=0

# Test function
run_test() {
    local test_name="$1"
    local args="$2"
    local expect_error="${3:-0}"
    
    TEST_COUNT=$((TEST_COUNT + 1))
    
    echo "=== Test #${TEST_COUNT}: ${test_name} ===" >> "$OUTPUT_FILE"
    echo "Command: ls ${args}" >> "$OUTPUT_FILE"
    echo "" >> "$OUTPUT_FILE"
    
    echo "Test #${TEST_COUNT}: ${test_name}"
    echo "  Command: ls ${args}"
    
    # Execute command
    if output=$(eval "ls ${args}" 2>&1); then
        exit_code=0
    else
        exit_code=$?
    fi
    
    # Record output
    if [ -n "$output" ]; then
        echo "Output:" >> "$OUTPUT_FILE"
        echo "$output" >> "$OUTPUT_FILE"
        
        # Show first 3 lines
        echo "  Output (first 3 lines):"
        echo "$output" | head -3 | while IFS= read -r line; do
            echo "    $line"
        done
        if [ "$(echo "$output" | wc -l)" -gt 3 ]; then
            echo "    ... (more in file)"
        fi
    else
        echo "(No output)" >> "$OUTPUT_FILE"
    fi
    
    # Check exit code
    if [ $exit_code -ne 0 ] && [ $expect_error -eq 0 ]; then
        ERROR_COUNT=$((ERROR_COUNT + 1))
        echo "Exit code: $exit_code" >> "$OUTPUT_FILE"
        echo "  Exit code: $exit_code" >&2
    fi
    
    echo "" >> "$OUTPUT_FILE"
    echo ""
}

echo "=== Running tests ==="
echo ""

# ==================== 1. Basic functionality tests ====================
echo "=== 1. Basic functionality ===" >> "$OUTPUT_FILE"
run_test "Basic listing" ""
run_test "Show all files" "-a"
run_test "Almost all (no . and ..)" "-A"
run_test "Ignore backups" "-B"
run_test "Long format" "-l"
run_test "Long format with all" "-la"
run_test "Human readable sizes" "-lh"
run_test "Long with human readable and all" "-lha"

# ==================== 2. Sorting tests ====================
echo "=== 2. Sorting tests ===" >> "$OUTPUT_FILE"
run_test "Sort by time" "-t"
run_test "Sort by size" "-S"
run_test "Reverse sort" "-r"
run_test "Long format sorted by time" "-lt"
run_test "Long format sorted by size" "-lS"
run_test "Long format sorted by time reverse" "-ltr"
run_test "Long format sorted by size reverse" "-lSr"
run_test "No sorting" "-U"
run_test "No sorting with all" "-Ua"

# ==================== 3. Display options tests ====================
echo "=== 3. Display options ===" >> "$OUTPUT_FILE"
run_test "One per line" "-1"
run_test "Show inode numbers" "-i"
run_test "Long with inode" "-li"
run_test "Show block sizes" "-s"
run_test "Long with block sizes" "-ls"
run_test "Kibibytes block size" "-sk"
run_test "Long with kibibytes" "-lsk"
run_test "Show directories only" "-d"
run_test "Long format directories only" "-ld"
run_test "Columns format" "-C"
run_test "Horizontal format" "-x"
run_test "Comma separated" "-m"

# ==================== 4. Path handling tests ====================
echo "=== 4. Path handling ===" >> "$OUTPUT_FILE"
run_test "Current directory (.)" "."
run_test "Parent directory (..)" ".."
run_test "Specific file" "normal.txt"
run_test "Long format specific file" "-l normal.txt"
run_test "Hidden file" ".hidden.txt"
run_test "Multiple files" "normal.txt file1.txt"
run_test "Directory" "files_dir"
run_test "Long format directory" "-l files_dir"
run_test "Empty directory" "empty_dir"
run_test "Deep directory" "deep"
run_test "File with spaces" "'file with spaces.txt'"
run_test "Multiple paths" "normal.txt files_dir empty_dir"
run_test "Wildcard pattern" "*.txt"
run_test "Wildcard with directories" "*/"

# ==================== 5. Special options tests ====================
echo "=== 5. Special options ===" >> "$OUTPUT_FILE"
run_test "Classify files" "-F"
run_test "Long with classify" "-lF"
run_test "Slash indicator" "-p"
run_test "Quote names" "-Q"
run_test "Literal (no quoting)" "-N"
run_test "Escape nongraphic" "-b"
run_test "Hide control chars" "-q"
run_test "Numeric UID/GID" "-n"
run_test "No owner" "-g"
run_test "No group" "-o"
run_test "Show author" "--author"
run_test "Show context" "-Z"
run_test "Dereference symlinks" "-L"
run_test "Dereference cmdline" "-H"
run_test "Natural sort" "-v"
run_test "Sort by extension" "-X"
run_test "Width option" "-w 80"
run_test "Tab size" "-T 4"

# ==================== 6. Time options tests ====================
echo "=== 6. Time options ===" >> "$OUTPUT_FILE"
run_test "Sort by ctime" "-c"
run_test "Sort by atime" "-u"
run_test "Long with ctime" "-lc"
run_test "Long with atime" "-lu"
run_test "Long sorted by ctime" "-ltc"
run_test "Long sorted by atime" "-ltu"
run_test "Time style locale" "--time-style=locale"
run_test "Time style long-iso" "--time-style=long-iso"
run_test "Time style full-iso" "--time-style=full-iso"
run_test "Time style iso" "--time-style=iso"

# ==================== 7. Color and format tests ====================
echo "=== 7. Color and format tests ===" >> "$OUTPUT_FILE"
run_test "Color auto" "--color=auto"
run_test "Color always" "--color=always"
run_test "Color never" "--color=never"
run_test "Indicator style none" "--indicator-style=none"
run_test "Indicator style slash" "--indicator-style=slash"
run_test "Indicator style file-type" "--indicator-style=file-type"
run_test "Indicator style classify" "--indicator-style=classify"
run_test "Hyperlink auto" "--hyperlink=auto"
run_test "Quoting style literal" "--quoting-style=literal"
run_test "Quoting style shell" "--quoting-style=shell"
run_test "Quoting style escape" "--quoting-style=escape"

# ==================== 8. Combined options tests ====================
echo "=== 8. Combined options ===" >> "$OUTPUT_FILE"
run_test "All combined 1" "-larth"
run_test "All combined 2" "-laSrh"
run_test "All combined 3" "-lathF"
run_test "All combined 4" "-laSrhF"
run_test "All combined 5" "-laSrhF --color=always"
run_test "All combined 6" "-laSrhF --time-style=long-iso"

# ==================== 9. Recursive tests ====================
echo "=== 9. Recursive tests ===" >> "$OUTPUT_FILE"
run_test "Recursive listing" "-R"
run_test "Recursive long format" "-lR"
run_test "Recursive with all" "-laR"
run_test "Recursive specific dir" "-R files_dir"
run_test "Recursive with sort" "-lR -t"

# ==================== 10. Error cases tests ====================
echo "=== 10. Error cases ===" >> "$OUTPUT_FILE"
run_test "Non-existent file" "nonexistent.txt" 1
run_test "Non-existent directory" "nonexistent_dir" 1
run_test "Invalid option" "-z" 1
run_test "Malformed option" "--invalid" 1

# ==================== 11. GNU extensions tests ====================
echo "=== 11. GNU extensions ===" >> "$OUTPUT_FILE"
run_test "Group directories first" "--group-directories-first"
run_test "Hide pattern" "--hide=*.txt"
run_test "Ignore pattern" "-I '*.txt'"
run_test "Sort by version" "--sort=version"
run_test "Sort by width" "--sort=width"
run_test "Sort by none" "--sort=none"
run_test "Time modification" "--time=mtime"
run_test "Time access" "--time=atime"
run_test "Time status" "--time=ctime"
run_test "Format verbose" "--format=verbose"
run_test "Format long" "--format=long"
run_test "Format commas" "--format=commas"
run_test "Format horizontal" "--format=horizontal"
run_test "Format vertical" "--format=vertical"
run_test "Format single-column" "--format=single-column"
run_test "Block size human" "--block-size=human"
run_test "Block size SI" "--block-size=si"
run_test "Zero terminator" "--zero"
run_test "Show control chars" "--show-control-chars"

# ==================== Cleanup and summary ====================
cd ..
rm -rf "$TEST_DIR"

echo "=== Test completed ===" >> "$OUTPUT_FILE"
echo "Completed: $(date)" >> "$OUTPUT_FILE"

# Summary
echo "" >> "$OUTPUT_FILE"
echo "=== Summary ===" >> "$OUTPUT_FILE"
echo "Total tests run: $TEST_COUNT" >> "$OUTPUT_FILE"
echo "Errors found: $ERROR_COUNT" >> "$OUTPUT_FILE"

echo ""
echo "=== Test completed ==="
echo "Total tests run: $TEST_COUNT"
echo "Errors found: $ERROR_COUNT"
echo "Results saved to: $OUTPUT_FILE"

# Show results preview
echo ""
echo "=== First 20 lines of results ==="
head -20 "$OUTPUT_FILE"

echo ""
echo "=== Last 10 lines of results ==="
tail -10 "$OUTPUT_FILE"