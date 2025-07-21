#!/usr/bin/env pwsh

# Tinky Build and Run Script
# This script sets up the Visual Studio Build Tools environment, builds only the tinky project, and runs it

Write-Host "Setting up Visual Studio Build Tools environment..." -ForegroundColor Green

# Change to tinky directory and run nmake, then execute the result
$result = & cmd /c "`"C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat`" && cd tinky && nmake $args"

if ($LASTEXITCODE -eq 0) {
    Write-Host "Tinky build completed successfully!" -ForegroundColor Green
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
} else {
    Write-Host "Tinky build failed with exit code: $LASTEXITCODE" -ForegroundColor Red
    exit $LASTEXITCODE
}
