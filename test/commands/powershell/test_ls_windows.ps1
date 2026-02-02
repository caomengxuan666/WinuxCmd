# test_ls_comprehensive.ps1 - Comprehensive ls command test

# Save current directory
$originalDir = Get-Location

# Output file - using absolute path
$OUTPUT_FILE = Join-Path $originalDir "ls_powershell_results.txt"

# Clear output file
"=== Windows ls Test Results ===" | Out-File $OUTPUT_FILE -Encoding UTF8 -Force
"Test started: $(Get-Date)" | Out-File $OUTPUT_FILE -Encoding UTF8 -Append
"" | Out-File $OUTPUT_FILE -Encoding UTF8 -Append

"Original directory: $originalDir" | Out-File $OUTPUT_FILE -Encoding UTF8 -Append

# Create test directory structure
$testDir = Join-Path $originalDir "ls_test_comprehensive"
Remove-Item -Path $testDir -Recurse -Force -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Path $testDir -Force | Out-Null

# Switch to test directory
Set-Location $testDir
"Test directory: $(Get-Location)" | Out-File $OUTPUT_FILE -Encoding UTF8 -Append
"" | Out-File $OUTPUT_FILE -Encoding UTF8 -Append

# Create complex directory structure
Write-Host "Creating test directory structure..." -ForegroundColor Green

# Create various files
$files = @(
    "normal.txt",
    ".hidden.txt",
    "backup.txt~",
    "executable.bat",
    "large_file.dat",
    "file with spaces.txt",
    "UPPERCASE.TXT",
    "lowercase.txt",
    "MixedCase.txt",
    "file1.txt",
    "file2.txt",
    "file3.txt"
)

foreach ($file in $files) {
    New-Item -ItemType File -Path $file -Force | Out-Null
}

# Create subdirectories and files
New-Item -ItemType Directory -Path "empty_dir" -Force | Out-Null
New-Item -ItemType Directory -Path "files_dir" -Force | Out-Null
New-Item -ItemType Directory -Path "deep\level1\level2" -Force | Out-Null

# Create files in subdirectories
1..3 | ForEach-Object {
    New-Item -ItemType File -Path "files_dir\file$_.txt" -Force | Out-Null
}

# Write different sizes of content
"Small file" | Set-Content "normal.txt"
"Hidden content" | Set-Content ".hidden.txt"
"Backup file" | Set-Content "backup.txt~"
"@echo off`necho Executable" | Set-Content "executable.bat"

# Create large file (50KB)
$largeContent = "X" * 51200  # 50KB
$largeContent | Set-Content "large_file.dat"

# Create file with spaces
"File with spaces" | Set-Content "file with spaces.txt"

# Set different modification times (by modifying after creation)
$oldDate = (Get-Date).AddDays(-30)
(Get-Item "file1.txt").LastWriteTime = $oldDate
(Get-Item "file2.txt").LastWriteTime = (Get-Date).AddDays(-15)
(Get-Item "file3.txt").LastWriteTime = (Get-Date).AddDays(-7)

Write-Host "Test structure created." -ForegroundColor Green
Write-Host ""

# ls.exe path - try multiple possible paths
$lsExe = $null
$possiblePaths = @(
    "$originalDir\ls.exe",
    "$originalDir\cmake-build-release\ls.exe",
    "$originalDir\build\ls.exe",
    "$originalDir\cmake-build-debug\ls.exe"
)

foreach ($path in $possiblePaths) {
    if (Test-Path $path) {
        $lsExe = $path
        break
    }
}

if (-Not $lsExe) {
    Write-Host "Error: ls.exe not found in any of the expected locations" -ForegroundColor Red
    "Error: ls.exe not found" | Out-File $OUTPUT_FILE -Encoding UTF8 -Append
    exit 1
}

"Using ls.exe from: $lsExe" | Out-File $OUTPUT_FILE -Encoding UTF8 -Append
Write-Host "Using ls.exe from: $lsExe" -ForegroundColor Yellow
Write-Host ""

# Global test counters
$global:testCount = 0
$global:errorCount = 0

# Test function
function Test-Ls {
    param(
        [string]$Description,
        [string]$Arguments,
        [switch]$ExpectError = $false
    )
    
    $global:testCount++
    $testNum = $global:testCount
    
    Write-Host "Test #${testNum}: $Description" -ForegroundColor Cyan
    Write-Host "Command: ls $Arguments" -ForegroundColor White
    
    "=== Test #${testNum}: $Description ===" | Out-File $OUTPUT_FILE -Encoding UTF8 -Append
    "Command: ls $Arguments" | Out-File $OUTPUT_FILE -Encoding UTF8 -Append
    "" | Out-File $OUTPUT_FILE -Encoding UTF8 -Append
    
    try {
        # Correctly handle quoted arguments
        $cmd = "& '$lsExe' $Arguments"
        $output = Invoke-Expression $cmd 2>&1
        $exitCode = $LASTEXITCODE
        
        if ($exitCode -ne 0 -and -not $ExpectError) {
            $global:errorCount++
            Write-Host "ERROR: Exit code $exitCode" -ForegroundColor Red
            "ERROR: Exit code $exitCode" | Out-File $OUTPUT_FILE -Encoding UTF8 -Append
        }
        
        if ($output) {
            Write-Host "Output:" -ForegroundColor Gray
            $output
            "Output:" | Out-File $OUTPUT_FILE -Encoding UTF8 -Append
            $output | Out-File $OUTPUT_FILE -Encoding UTF8 -Append
        } else {
            "(No output)" | Out-File $OUTPUT_FILE -Encoding UTF8 -Append
        }
    } catch {
        $global:errorCount++
        $errorMsg = "ERROR: $_"
        Write-Host $errorMsg -ForegroundColor Red
        $errorMsg | Out-File $OUTPUT_FILE -Encoding UTF8 -Append
    }
    
    "" | Out-File $OUTPUT_FILE -Encoding UTF8 -Append
    Write-Host ""
}

# ==================== Basic functionality tests ====================
Write-Host "=== 1. Basic functionality ===" -ForegroundColor Magenta
"=== 1. Basic functionality ===" | Out-File $OUTPUT_FILE -Encoding UTF8 -Append

Test-Ls "Basic listing (no args)" ""
Test-Ls "Show all files" "-a"
Test-Ls "Almost all (no . and ..)" "-A"
Test-Ls "Ignore backups" "-B"
Test-Ls "Long format" "-l"
Test-Ls "Long format with all" "-la"
Test-Ls "Human readable sizes" "-lh"
Test-Ls "Long with human readable" "-lha"

# ==================== Sorting tests ====================
Write-Host "=== 2. Sorting tests ===" -ForegroundColor Magenta
"=== 2. Sorting tests ===" | Out-File $OUTPUT_FILE -Encoding UTF8 -Append

Test-Ls "Sort by name (default)" ""
Test-Ls "Sort by time" "-t"
Test-Ls "Sort by size" "-S"
Test-Ls "Reverse sort" "-r"
Test-Ls "Long format sorted by time" "-lt"
Test-Ls "Long format sorted by size" "-lS"
Test-Ls "Long format sorted by time reverse" "-ltr"
Test-Ls "Long format sorted by size reverse" "-lSr"
Test-Ls "No sorting" "-U"
Test-Ls "No sorting with all" "-Ua"

# ==================== Display options tests ====================
Write-Host "=== 3. Display options ===" -ForegroundColor Magenta
"=== 3. Display options ===" | Out-File $OUTPUT_FILE -Encoding UTF8 -Append

Test-Ls "One per line" "-1"
Test-Ls "Show inode numbers" "-i"
Test-Ls "Long with inode" "-li"
Test-Ls "Show block sizes" "-s"
Test-Ls "Long with block sizes" "-ls"
Test-Ls "Kibibytes block size" "-sk"
Test-Ls "Long with kibibytes" "-lsk"
Test-Ls "Show directories only" "-d"
Test-Ls "Long format directories only" "-ld"

# ==================== Path handling tests ====================
Write-Host "=== 4. Path handling ===" -ForegroundColor Magenta
"=== 4. Path handling ===" | Out-File $OUTPUT_FILE -Encoding UTF8 -Append

Test-Ls "Current directory (.)" "."
Test-Ls "Parent directory (..)" ".."
Test-Ls "Specific file" "normal.txt"
Test-Ls "Long format specific file" "-l normal.txt"
Test-Ls "Hidden file" ".hidden.txt"
Test-Ls "Multiple files" "normal.txt file1.txt"
Test-Ls "Directory" "files_dir"
Test-Ls "Long format directory" "-l files_dir"
Test-Ls "Empty directory" "empty_dir"
Test-Ls "Deep directory" "deep"
Test-Ls "File with spaces" "`"file with spaces.txt`""
Test-Ls "Multiple paths" "normal.txt files_dir empty_dir"

# ==================== Special options tests ====================
Write-Host "=== 5. Special options ===" -ForegroundColor Magenta
"=== 5. Special options ===" | Out-File $OUTPUT_FILE -Encoding UTF8 -Append

Test-Ls "Classify files" "-F"
Test-Ls "Long with classify" "-lF"
Test-Ls "Slash indicator" "-p"
Test-Ls "Quote names" "-Q"
Test-Ls "Literal (no quoting)" "-N"
Test-Ls "Escape nongraphic" "-b"
Test-Ls "Hide control chars" "-q"
Test-Ls "Numeric UID/GID" "-n"
Test-Ls "No owner" "-g"
Test-Ls "No group" "-o"

# ==================== Combined options tests ====================
Write-Host "=== 6. Combined options ===" -ForegroundColor Magenta
"=== 6. Combined options ===" | Out-File $OUTPUT_FILE -Encoding UTF8 -Append

Test-Ls "All combined 1" "-larth"
Test-Ls "All combined 2" "-laSrh"
Test-Ls "All combined 3" "-lathF"
Test-Ls "All combined 4" "-laSrhF"

# ==================== Recursive tests ====================
Write-Host "=== 7. Recursive tests ===" -ForegroundColor Magenta
"=== 7. Recursive tests ===" | Out-File $OUTPUT_FILE -Encoding UTF8 -Append

Test-Ls "Recursive listing" "-R"
Test-Ls "Recursive long format" "-lR"
Test-Ls "Recursive with all" "-laR"
Test-Ls "Recursive specific dir" "-R files_dir"

# ==================== Error cases tests ====================
Write-Host "=== 8. Error cases ===" -ForegroundColor Magenta
"=== 8. Error cases ===" | Out-File $OUTPUT_FILE -Encoding UTF8 -Append

Test-Ls "Non-existent file" "nonexistent.txt" -ExpectError
Test-Ls "Non-existent directory" "nonexistent_dir" -ExpectError
Test-Ls "Invalid option" "-z" -ExpectError
Test-Ls "Malformed option" "--invalid" -ExpectError

# ==================== Comparison with system dir ====================
Write-Host "=== 9. Comparison with system dir ===" -ForegroundColor Magenta
"=== 9. Comparison with system dir ===" | Out-File $OUTPUT_FILE -Encoding UTF8 -Append

if (Get-Command dir -ErrorAction SilentlyContinue) {
    Write-Host "Comparing with system 'dir' command:" -ForegroundColor Yellow
    "Comparing with system 'Get-ChildItem' command:" | Out-File $OUTPUT_FILE -Encoding UTF8 -Append
    
    Write-Host "`nWinuxCmd ls -la:"
    "`nWinuxCmd ls -la:" | Out-File $OUTPUT_FILE -Encoding UTF8 -Append
    & $lsExe -la | Select-Object -First 5
    & $lsExe -la | Select-Object -First 5 | Out-File $OUTPUT_FILE -Encoding UTF8 -Append
    
    Write-Host "`nSystem Get-ChildItem -Force:"  
    "`nSystem Get-ChildItem -Force:" | Out-File $OUTPUT_FILE -Encoding UTF8 -Append
    Get-ChildItem -Force | Select-Object -First 5
    Get-ChildItem -Force | Select-Object -First 5 | Out-File $OUTPUT_FILE -Encoding UTF8 -Append
}

# ==================== Cleanup ====================
Write-Host "`n=== Cleaning up ===" -ForegroundColor Green
"`n=== Cleaning up ===" | Out-File $OUTPUT_FILE -Encoding UTF8 -Append
Set-Location $originalDir
Remove-Item -Path $testDir -Recurse -Force

Write-Host "`n=== All tests completed ===" -ForegroundColor Cyan
"`n=== All tests completed ===" | Out-File $OUTPUT_FILE -Encoding UTF8 -Append
Write-Host "Test directory: $testDir has been cleaned up." -ForegroundColor Green
"Test directory: $testDir has been cleaned up." | Out-File $OUTPUT_FILE -Encoding UTF8 -Append

# Summary
Write-Host "`n=== Summary ===" -ForegroundColor Yellow
"`n=== Summary ===" | Out-File $OUTPUT_FILE -Encoding UTF8 -Append
Write-Host "Total tests run: $global:testCount"
"Total tests run: $global:testCount" | Out-File $OUTPUT_FILE -Encoding UTF8 -Append
Write-Host "Errors found: $global:errorCount"
"Errors found: $global:errorCount" | Out-File $OUTPUT_FILE -Encoding UTF8 -Append
Write-Host "Results saved to: $OUTPUT_FILE"
"Results saved to: $OUTPUT_FILE" | Out-File $OUTPUT_FILE -Encoding UTF8 -Append
Write-Host "Check above output for any errors or unexpected behavior."
Write-Host "Compare with Linux ls output for differences."

# Show results preview
Write-Host "`n=== First 20 lines of results ==="
Get-Content $OUTPUT_FILE -TotalCount 20 | ForEach-Object {
    Write-Host $_ -ForegroundColor Gray
}

Write-Host "`n=== Last 10 lines of results ==="
Get-Content $OUTPUT_FILE | Select-Object -Last 10 | ForEach-Object {
    Write-Host $_ -ForegroundColor Gray
}
