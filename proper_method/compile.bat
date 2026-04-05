@echo off
echo.
echo [RAID COMPILER] Compiling project files...
echo.

gcc main.c raid.c disk.c -o ergasia1.exe

if %ERRORLEVEL% EQU 0 (
    echo.
    echo [SUCCESS] Compilation complete. 
    echo Usage: ergasia1.exe RAIDX SIZE BLOCKSIZE N inputFile updates backup allData
    echo.
) else (
    echo.
    echo [ERROR] Compilation failed. Please ensure GCC is installed.
    echo.
)
pause
