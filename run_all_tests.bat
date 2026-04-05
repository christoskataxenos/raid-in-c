@echo off
setlocal enabledelayedexpansion

:: Set current directory to script location
cd /d "%~dp0"

echo.
echo ============================================================
echo [RAID SIMULATOR TEST SUITE]
echo ============================================================
echo.
echo Select the version you want to compile and test:
echo [1] Academic Method (Single File)
echo [2] Proper Method (Modular Files)
echo.
set /p choice="Enter your choice (1 or 2): "

if "%choice%"=="1" (
    set EXE=akadimaiki_method\ergasia1.exe
    set SRC=akadimaiki_method\raid_simple.c
    echo.
    echo [RUNNING] Compiling Academic Method...
    gcc !SRC! -o !EXE!
) else if "%choice%"=="2" (
    set EXE=proper_method\ergasia1.exe
    set SRC=proper_method\main.c proper_method\raid.c proper_method\disk.c
    echo.
    echo [RUNNING] Compiling Proper Method...
    gcc !SRC! -o !EXE!
) else (
    echo [ERROR] Invalid choice.
    pause
    exit /b
)

:: Check if compilation was successful
if %ERRORLEVEL% NEQ 0 (
    echo.
    echo [ERROR] Compilation failed! Please check your GCC installation.
    pause
    exit /b
)

echo [SUCCESS] Compilation complete. Starting test scenarios...
echo.
pause

:: SCENARIO 1: RAID 1 (Mirroring)
cls
echo --- [SCENARIO 1: RAID 1] ---
echo RAID 1, SIZE=30, BLOCK=5, N=2, input1.txt
echo 1 ^| "%EXE%" RAID1 30 5 2 input_and_update_files\input1.txt input_and_update_files\update1.txt backup.txt allData.txt
echo 1 | "%EXE%" RAID1 30 5 2 input_and_update_files\input1.txt input_and_update_files\update1.txt backup.txt allData.txt
echo.
echo [!] Press any key for the next scenario...
pause > nul

:: SCENARIO 2: RAID 4 (Fixed Parity)
cls
echo --- [SCENARIO 2: RAID 4] ---
echo RAID 4, SIZE=25, BLOCK=5, N=3, input1.txt
echo 1 ^| "%EXE%" RAID4 25 5 3 input_and_update_files\input1.txt input_and_update_files\update1.txt backup.txt allData.txt
echo 1 | "%EXE%" RAID4 25 5 3 input_and_update_files\input1.txt input_and_update_files\update1.txt backup.txt allData.txt
echo.
echo [!] Press any key for the next scenario...
pause > nul

:: SCENARIO 3: RAID 1E (Enhanced Mirroring)
cls
echo --- [SCENARIO 3: RAID 1E] ---
echo RAID 1E, SIZE=25, BLOCK=5, N=3, input1.txt
echo 0 ^| "%EXE%" RAID1E 25 5 3 input_and_update_files\input1.txt input_and_update_files\update1.txt backup.txt allData.txt
echo 0 | "%EXE%" RAID1E 25 5 3 input_and_update_files\input1.txt input_and_update_files\update1.txt backup.txt allData.txt
echo.
echo [!] Press any key for the next scenario...
pause > nul

:: SCENARIO 4: RAID 5 (Distributed Parity)
cls
echo --- [SCENARIO 4: RAID 5] ---
echo RAID 5, SIZE=300, BLOCK=10, N=5, input2.txt
echo 4 ^| "%EXE%" RAID5 300 10 5 input_and_update_files\input2.txt input_and_update_files\update2.txt backup.txt allData.txt
echo 4 | "%EXE%" RAID5 300 10 5 input_and_update_files\input2.txt input_and_update_files\update2.txt backup.txt allData.txt
echo.

echo ============================================================
echo [TESTS COMPLETE] Check "100%% similarity" in each scenario.
echo ============================================================
pause
