#!/usr/bin/env pwsh

# Developer Command Prompt Build Script
# This script sets up the Visual Studio Build Tools environment and runs nmake

Write-Host "Setting up Visual Studio Build Tools environment..." -ForegroundColor Green

# Run vcvars64.bat and then nmake
& cmd /c "`"C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat`" && nmake $args"

if ($LASTEXITCODE -eq 0) {
    Write-Host "Build completed successfully!" -ForegroundColor Green
} else {
    Write-Host "Build failed with exit code: $LASTEXITCODE" -ForegroundColor Red
    exit $LASTEXITCODE
}
