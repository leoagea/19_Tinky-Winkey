@echo off
echo Setting up Visual Studio Build Tools environment...
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

echo Building tinky project...
cd tinky
nmake %*
if %ERRORLEVEL% neq 0 (
    echo Build failed with exit code: %ERRORLEVEL%
    exit /b %ERRORLEVEL%
)

echo.
echo Tinky build completed successfully!
echo Running tinky executable...
echo =========================
svc.exe
set EXEC_RESULT=%ERRORLEVEL%
echo =========================
if %EXEC_RESULT% equ 0 (
    echo Tinky execution completed successfully!
) else (
    echo Tinky execution failed with exit code: %EXEC_RESULT%
    exit /b %EXEC_RESULT%
)
