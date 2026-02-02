# diff_ls_results.ps1
# Compare two ls test result files

$file1 = "ls_bash_results.txt"
$file2 = "ls_powershell_results.txt"

# Check if files exist
if (!(Test-Path $file1)) {
    Write-Host "Error: File $file1 not found" -ForegroundColor Red
    exit 1
}

if (!(Test-Path $file2)) {
    Write-Host "Error: File $file2 not found" -ForegroundColor Red
    exit 1
}

# Basic information
Write-Host "=== File Comparison ===" -ForegroundColor Cyan
Write-Host "File 1: $file1" -ForegroundColor Yellow
Write-Host "Size: $((Get-Item $file1).Length) bytes"
Write-Host "Lines: $((Get-Content $file1 | Measure-Object -Line).Lines)" 
Write-Host ""
Write-Host "File 2: $file2" -ForegroundColor Yellow
Write-Host "Size: $((Get-Item $file2).Length) bytes"
Write-Host "Lines: $((Get-Content $file2 | Measure-Object -Line).Lines)"
Write-Host ""

# 1. Content difference comparison
Write-Host "=== Content Differences ===" -ForegroundColor Cyan

# Compare two files using Compare-Object
$content1 = Get-Content $file1
$content2 = Get-Content $file2

$diff = Compare-Object -ReferenceObject $content1 -DifferenceObject $content2

if ($diff) {
    Write-Host "Found $($diff.Count) differences:" -ForegroundColor Red
    
    # Group and display differences
    $leftOnly = $diff | Where-Object { $_.SideIndicator -eq "<=" }
    $rightOnly = $diff | Where-Object { $_.SideIndicator -eq "=>" }
    
    if ($leftOnly) {
        Write-Host "`nContent only in $file1 ($($leftOnly.Count) lines):" -ForegroundColor Magenta
        $leftOnly | Select-Object -First 10 | ForEach-Object {
            Write-Host "< $($_.InputObject)" -ForegroundColor Red
        }
        if ($leftOnly.Count -gt 10) {
            Write-Host "... (and $($leftOnly.Count - 10) more lines)" -ForegroundColor Gray
        }
    }
    
    if ($rightOnly) {
        Write-Host "`nContent only in $file2 ($($rightOnly.Count) lines):" -ForegroundColor Magenta
        $rightOnly | Select-Object -First 10 | ForEach-Object {
            Write-Host "> $($_.InputObject)" -ForegroundColor Green
        }
        if ($rightOnly.Count -gt 10) {
            Write-Host "... (and $($rightOnly.Count - 10) more lines)" -ForegroundColor Gray
        }
    }
} else {
    Write-Host "Both files are identical!" -ForegroundColor Green
}

Write-Host ""

# 2. Statistics comparison
Write-Host "=== Statistics ===" -ForegroundColor Cyan

# Count number of tests
$testCount1 = ($content1 | Select-String "=== Test #" | Measure-Object).Count
$testCount2 = ($content2 | Select-String "=== Test #" | Measure-Object).Count

# Count number of errors
$errorCount1 = ($content1 | Select-String "ERROR:" | Measure-Object).Count
$errorCount2 = ($content2 | Select-String "ERROR:" | Measure-Object).Count

# Count number of commands
$commandCount1 = ($content1 | Select-String "Command: ls" | Measure-Object).Count
$commandCount2 = ($content2 | Select-String "Command: ls" | Measure-Object).Count

Write-Host "Test count: $testCount1 vs $testCount2" -ForegroundColor White
Write-Host "Error count: $errorCount1 vs $errorCount2" -ForegroundColor White
Write-Host "Command count: $commandCount1 vs $commandCount2" -ForegroundColor White

# 3. Start and end comparison
Write-Host "`n=== File Start Comparison ===" -ForegroundColor Cyan
Write-Host "First 5 lines of ${file1}:" -ForegroundColor Yellow
Get-Content $file1 -TotalCount 5 | ForEach-Object { Write-Host "  $_" }

Write-Host "`nFirst 5 lines of ${file2}:" -ForegroundColor Yellow
Get-Content $file2 -TotalCount 5 | ForEach-Object { Write-Host "  $_" }

Write-Host "`n=== File End Comparison ===" -ForegroundColor Cyan
Write-Host "Last 5 lines of ${file1}:" -ForegroundColor Yellow
Get-Content $file1 | Select-Object -Last 5 | ForEach-Object { Write-Host "  $_" }

Write-Host "`nLast 5 lines of ${file2}:" -ForegroundColor Yellow
Get-Content $file2 | Select-Object -Last 5 | ForEach-Object { Write-Host "  $_" }

# 4. Key information extraction
Write-Host "`n=== Key Test Results ===" -ForegroundColor Cyan

# Extract from bash results
$bashSummary = $content1 | Select-String -Pattern "Total tests run:|Errors found:" | Select-Object -Last 2
if ($bashSummary) {
    Write-Host "Bash test results:" -ForegroundColor Yellow
    $bashSummary | ForEach-Object { Write-Host "  $_" }
}

# Extract from PowerShell results
$psSummary = $content2 | Select-String -Pattern "Total tests run:|Errors found:" | Select-Object -Last 2
if ($psSummary) {
    Write-Host "PowerShell test results:" -ForegroundColor Yellow
    $psSummary | ForEach-Object { Write-Host "  $_" }
}

# 5. Save difference report
$diffFile = "ls_results_diff.txt"
Write-Host "`n=== Generating difference report: $diffFile ===" -ForegroundColor Cyan

$report = @"
=== LS Test Results Difference Report ===
Generated: $(Get-Date)
File 1: $file1 ($((Get-Item $file1).Length) bytes, $((Get-Content $file1 | Measure-Object -Line).Lines) lines)
File 2: $file2 ($((Get-Item $file2).Length) bytes, $((Get-Content $file2 | Measure-Object -Line).Lines) lines)

=== Statistical Differences ===
Test count difference: $testCount1 vs $testCount2
Error count difference: $errorCount1 vs $errorCount2

=== Detailed Differences ===
"@

if ($diff) {
    $report += "`nFound $($diff.Count) differences:"
    $report += "`nContent only in ${file1}:"
    $leftOnly | ForEach-Object { $report += "`n< $($_.InputObject)" }
    $report += "`n`nContent only in ${file2}:"
    $rightOnly | ForEach-Object { $report += "`n> $($_.InputObject)" }
} else {
    $report += "`nBoth files are identical"
}

$report | Out-File $diffFile -Encoding UTF8

Write-Host "Difference report saved to: $diffFile" -ForegroundColor Green