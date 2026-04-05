@echo off
:: Μετάβαση στον φάκελο που βρίσκεται το script
cd /d "%~dp0"

set EXE=akadimaiki_method\ergasia1.exe
echo.
echo ============================================================
echo [RAID SIMULATOR TEST SUITE]
echo ============================================================
echo.

:: Έλεγχος αν υπάρχει το αρχείο
if not exist "%EXE%" (
    echo [ERROR] Το αρχείο "%EXE%" δεν βρέθηκε!
    echo Βεβαιωθείτε ότι το αρχείο υπάρχει στον φάκελο: %CD%\akadimaiki_method
    echo.
    echo Τρέξτε πρώτα: 
    echo gcc akadimaiki_method\raid_simple.c -o akadimaiki_method\ergasia1.exe
    pause
    exit /b
)

:: SCENARIO 1: RAID 1 (Mirroring)
cls
echo --- [SCENARIO 1: RAID 1] ---
echo RAID 1, SIZE=30, BLOCK=5, N=2, input1.txt
echo 1 ^| "%EXE%" RAID1 30 5 2 input_and_update_files\input1.txt input_and_update_files\update1.txt backup.txt allData.txt
echo 1 | "%EXE%" RAID1 30 5 2 input_and_update_files\input1.txt input_and_update_files\update1.txt backup.txt allData.txt
echo.
echo [!] Πατήστε ένα πλήκτρο για να πάμε στο επόμενο σενάριο...
pause > nul

:: SCENARIO 2: RAID 4 (Fixed Parity)
cls
echo --- [SCENARIO 2: RAID 4] ---
echo RAID 4, SIZE=25, BLOCK=5, N=3, input1.txt
echo 1 ^| "%EXE%" RAID4 25 5 3 input_and_update_files\input1.txt input_and_update_files\update1.txt backup.txt allData.txt
echo 1 | "%EXE%" RAID4 25 5 3 input_and_update_files\input1.txt input_and_update_files\update1.txt backup.txt allData.txt
echo.
echo [!] Πατήστε ένα πλήκτρο για να πάμε στο επόμενο σενάριο...
pause > nul

:: SCENARIO 4: RAID 1E (Enhanced Mirroring)
cls
echo --- [SCENARIO 4: RAID 1E] ---
echo RAID 1E, SIZE=25, BLOCK=5, N=3, input1.txt
echo 0 ^| "%EXE%" RAID1E 25 5 3 input_and_update_files\input1.txt input_and_update_files\update1.txt backup.txt allData.txt
echo 0 | "%EXE%" RAID1E 25 5 3 input_and_update_files\input1.txt input_and_update_files\update1.txt backup.txt allData.txt
echo.
echo [!] Πατήστε ένα πλήκτρο για να πάμε στο επόμενο σενάριο...
pause > nul

:: SCENARIO 5: LARGE DATA TEST
cls
echo --- [SCENARIO 5: LARGE DATA] ---
echo RAID 5, SIZE=300, BLOCK=10, N=5, input2.txt
echo 4 ^| "%EXE%" RAID5 300 10 5 input_and_update_files\input2.txt input_and_update_files\update2.txt backup.txt allData.txt
echo 4 | "%EXE%" RAID5 300 10 5 input_and_update_files\input2.txt input_and_update_files\update2.txt backup.txt allData.txt
echo.

echo ============================================================
echo [TESTS COMPLETE] Check "100%% similarity" in each scenario.
echo ============================================================
pause
