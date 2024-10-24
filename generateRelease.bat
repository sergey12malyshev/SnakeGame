cls
@echo off
chcp 1251

echo generateRelease...

PowerShell.exe -noexit -executionpolicy bypass -executionpolicy bypass -File "scripts/generateRelease.ps1"

exit