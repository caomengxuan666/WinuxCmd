<#
.SYNOPSIS
    WinuxCmd One-Click Release Script
.DESCRIPTION
    Updates version numbers in all files and pushes to GitHub
#>

param(
    [Parameter(Mandatory=$true)]
    [string]$Version,

    [Parameter(Mandatory=$false)]
    [string]$CommitMessage = "release: version $Version",

    [Parameter(Mandatory=$false)]
    [switch]$SkipCommit,

    [Parameter(Mandatory=$false)]
    [switch]$PrepareOnly
)

$ErrorActionPreference = "Stop"

function Write-Color {
    param($Color, $Text)
    $Colors = @{
        Green  = "[OK]"
        Yellow = "[INFO]"
        Red    = "[ERROR]"
        Blue   = "[NOTE]"
        Cyan   = "[INFO]"
    }
    Write-Host "$($Colors[$Color]) $Text"
}

Write-Color "Cyan" "WinuxCmd One-Click Release Script"
Write-Color "Cyan" "================================"
Write-Host ""

# Validate version format
if ($Version -notmatch '^\d+\.\d+\.\d+$') {
    Write-Color "Red" "Invalid version format. Use semantic versioning (e.g., 0.4.5)"
    exit 1
}

Write-Color "Yellow" "Version: $Version"
Write-Host ""

# Update version sources
Write-Color "Yellow" "Updating version metadata..."
Set-Content -Path "PROJECT_VERSION" -Value $Version -Encoding ascii -NoNewline
Write-Color "Green" "  Updated PROJECT_VERSION"

$releaseDate = Get-Date -Format "yyyy-MM-dd"
$builtinIndexVersion = "builtin-" + $releaseDate.Replace("-", ".")
$wpmPath = "src\commands\wpm.cpp"
$wpm = Get-Content $wpmPath -Raw
$wpm = $wpm -replace '"version": "builtin-\d{4}\.\d{2}\.\d{2}"',
                    ('"version": "' + $builtinIndexVersion + '"')
$wpm = $wpm -replace '"updated": "\d{4}-\d{2}-\d{2}"',
                    ('"updated": "' + $releaseDate + '"')
$wpm = $wpm -replace '"version": "\d+\.\d+\.\d+",\r?\n      "description": "WinuxCmd core command set"',
                    ('"version": "' + $Version + '",' + "`n" + '      "description": "WinuxCmd core command set"')
Set-Content -Path $wpmPath -Value $wpm -Encoding utf8 -NoNewline
Write-Color "Green" "  Updated WPM builtin index metadata"

Write-Host ""

# Check git status
$gitStatus = git status --short
if (-not $gitStatus) {
    Write-Color "Yellow" "No changes detected. Files already at version $Version"
    exit 0
}

if ($PrepareOnly) {
    Write-Color "Yellow" "Prepare-only mode: version metadata updated; skipping commit, push, and tag"
    exit 0
}

Write-Color "Yellow" "Git changes detected:"
Write-Host $gitStatus
Write-Host ""

# Commit changes
if (-not $SkipCommit) {
    Write-Color "Yellow" "Committing changes..."
    git add -A
    git commit -m $CommitMessage
    Write-Color "Green" "  Committed successfully"
    Write-Host ""
} else {
    Write-Color "Yellow" "Skipping commit (--SkipCommit specified)"
    Write-Host ""
}

# Push to remote
Write-Color "Yellow" "Pushing to remote..."
git push origin main
Write-Color "Green" "  Pushed successfully"
Write-Host ""

# Create tag
Write-Color "Yellow" "Creating tag v$Version..."
git tag -a "v$Version" -m "Release v$Version"
Write-Color "Green" "  Tag created"
Write-Host ""

# Push tag
Write-Color "Yellow" "Pushing tag to remote..."
git push origin "v$Version"
Write-Color "Green" "  Tag pushed"
Write-Host ""

Write-Color "Cyan" "================================"
Write-Color "Green" "Release v$Version completed successfully!"
Write-Color "Cyan" "================================"
Write-Host ""
Write-Host "Next steps:"
Write-Host "1. Monitor GitHub Actions build: https://github.com/unixwin/WinuxCmd/actions"
Write-Host "2. Check release: https://github.com/unixwin/WinuxCmd/releases/tag/v$Version"
Write-Host ""
