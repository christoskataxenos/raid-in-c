@echo off
echo.
echo [RAID COMPILER] Compiling project files...
echo.

gcc main.c shell.c raid.c disk.c env.c -o raid.exe

if %ERRORLEVEL% EQU 0 (
    echo.
    echo [SUCCESS] Compilation complete. Execute 'raid.exe' to begin.
    echo.
) else (
    echo.
    echo [ERROR] Compilation failed. Please ensure GCC is installed.
    echo.
)
pause
