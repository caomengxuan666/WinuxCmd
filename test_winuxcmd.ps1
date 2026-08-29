# Test WinuxCmd commands from Windows side
$winuxCmd = 'D:\repo\unixwin-winuxcmd\build-vs\usr\bin'
$testDir = 'D:\repo\unixwin-winuxcmd\test_gnu'

New-Item -ItemType Directory -Path $testDir -Force | Out-Null

Set-Content -Path "$testDir\input1.txt" -Value 'aardvark' -NoNewline -Encoding ascii
Add-Content -Path "$testDir\input1.txt" -Value 'bear' -NoNewline -Encoding ascii
Add-Content -Path "$testDir\input1.txt" -Value 'cat' -NoNewline -Encoding ascii
Add-Content -Path "$testDir\input1.txt" -Value 'dog' -NoNewline -Encoding ascii

Write-Host 'Test files created'

$result = & "$winuxCmd\cat.exe" "$testDir\input1.txt" 2>&1
Write-Host "cat output: $result"
Write-Host "Exit: $LASTEXITCODE"
