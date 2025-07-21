#!/usr/bin/env pwsh

# Tinky Only Build Script
# This script sets up the Visual Studio Build Tools environment and runs nmake only for the tinky project

param(
    [string]$Target = ""
)

Write-Host "Setting up Visual Studio Build Tools environment..." -ForegroundColor Green

# Determine what to build based on parameters
$buildArgs = if ($Target) { $Target } else { "" }

# Run vcvars64.bat and then nmake in tinky directory
$result = & cmd /c "`"C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat`" && cd tinky && nmake $buildArgs"

if ($LASTEXITCODE -eq 0) {
    Write-Host "Tinky build completed successfully!" -ForegroundColor Green
    
    # Only run the executable if we're doing a normal build (not clean, fclean, etc.)
    if ($Target -eq "" -or $Target -eq "all") {
        Write-Host "Running tinky executable..." -ForegroundColor Cyan
        Write-Host "=========================" -ForegroundColor Yellow
        
        # Execute the tinky program
        & ".\tinky\svc.exe"
        
        $execResult = $LASTEXITCODE
        Write-Host "=========================" -ForegroundColor Yellow
        
        if ($execResult -eq 0) {
            Write-Host "Tinky execution completed successfully!" -ForegroundColor Green
        } else {
            Write-Host "Tinky execution failed with exit code: $execResult" -ForegroundColor Red
            exit $execResult
        }
    }
} else {
    Write-Host "Tinky build failed with exit code: $LASTEXITCODE" -ForegroundColor Red
    exit $LASTEXITCODE
}
