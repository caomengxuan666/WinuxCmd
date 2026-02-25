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
    [switch]$SkipCommit
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

# Files to update
$filesToUpdate = @{
    "CMakeLists.txt" = "VERSION 0.4.4", "VERSION $Version"
    "scripts\create_links.ps1" = "0.4.4", "$Version"
    "scripts\winuxcmd_init.cmd" = "0.4.4", "$Version"
}

# Update files
Write-Color "Yellow" "Updating version numbers..."
foreach ($file in $filesToUpdate.Keys) {
    $oldVersion = $filesToUpdate[$file]
    $newVersion = $filesToUpdate[$file + "_NEW"]
    
    $content = Get-Content $file -Raw
    if ($content -match $oldVersion) {
        $content = $content -replace $oldVersion, $newVersion
        Set-Content $file -Value $content -Encoding UTF8
        Write-Color "Green" "  Updated $file"
    } else {
        Write-Color "Yellow" "  Skipped $file (version not found or already updated)"
    }
}

Write-Host ""

# Check git status
$gitStatus = git status --short
if (-not $gitStatus) {
    Write-Color "Yellow" "No changes detected. Files already at version $Version"
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
git push origin develop
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
Write-Host "1. Monitor GitHub Actions build: https://github.com/caomengxuan666/WinuxCmd/actions"
Write-Host "2. Check release: https://github.com/caomengxuan666/WinuxCmd/releases/tag/v$Version"
Write-Host ""