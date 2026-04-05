#!/bin/bash

while true; do
    # Navigate to script directory
    cd "$(dirname "$0")"
    clear

    echo ""
    echo "============================================================"
    echo "[RAID SIMULATOR TEST SUITE]"
    echo "============================================================"
    echo ""
    echo "Select the version you want to compile and test:"
    echo "[1] Academic Method (Single File)"
    echo "[2] Proper Method (Modular Files)"
    echo "[3] Exit"
    echo ""
    read -p "Enter your choice (1, 2 or 3): " choice

    if [ "$choice" == "1" ]; then
        EXE="akadimaiki_method/ergasia1.exe"
        SRC="akadimaiki_method/raid_simple.c"
        echo ""
        echo "[RUNNING] Compiling Academic Method..."
        gcc $SRC -o $EXE
    elif [ "$choice" == "2" ]; then
        EXE="proper_method/ergasia1.exe"
        SRC="proper_method/main.c proper_method/raid.c proper_method/disk.c"
        echo ""
        echo "[RUNNING] Compiling Proper Method..."
        gcc $SRC -o $EXE
    elif [ "$choice" == "3" ]; then
        echo "[EXIT] Goodbye!"
        exit 0
    else
        echo "[ERROR] Invalid choice."
        read -p "Press Enter to try again..."
        continue
    fi

    # Check if compilation was successful
    if [ $? -ne 0 ]; then
        echo ""
        echo "[ERROR] Compilation failed! Please check your GCC installation."
        read -p "Press Enter to return to menu..."
        continue
    fi

    echo "[SUCCESS] Compilation complete. Starting test scenarios..."
    echo ""
    read -p "Press Enter to start tests..."

    # SCENARIO 1: RAID 1
    clear
    echo "--- [SCENARIO 1: RAID 1] ---"
    echo "RAID 1, SIZE=30, BLOCK=5, N=2, input1.txt"
    echo "1 | ./$EXE RAID1 30 5 2 input_and_update_files/input1.txt input_and_update_files/update1.txt backup.txt allData.txt"
    echo "1" | ./$EXE RAID1 30 5 2 input_and_update_files/input1.txt input_and_update_files/update1.txt backup.txt allData.txt
    echo ""
    read -p "[!] Press Enter for next scenario..."

    # SCENARIO 2: RAID 4
    clear
    echo "--- [SCENARIO 2: RAID 4] ---"
    echo "RAID 4, SIZE=25, BLOCK=5, N=3, input1.txt"
    echo "1 | ./$EXE RAID4 25 5 3 input_and_update_files/input1.txt input_and_update_files/update1.txt backup.txt allData.txt"
    echo "1" | ./$EXE RAID4 25 5 3 input_and_update_files/input1.txt input_and_update_files/update1.txt backup.txt allData.txt
    echo ""
    read -p "[!] Press Enter for next scenario..."

    # SCENARIO 3: RAID 1E
    clear
    echo "--- [SCENARIO 3: RAID 1E] ---"
    echo "RAID 1E, SIZE=25, BLOCK=5, N=3, input1.txt"
    echo "0 | ./$EXE RAID1E 25 5 3 input_and_update_files/input1.txt input_and_update_files/update1.txt backup.txt allData.txt"
    echo "0" | ./$EXE RAID1E 25 5 3 input_and_update_files/input1.txt input_and_update_files/update1.txt backup.txt allData.txt
    echo ""
    read -p "[!] Press Enter for next scenario..."

    # SCENARIO 4: RAID 5
    clear
    echo "--- [SCENARIO 4: RAID 5] ---"
    echo "RAID 5, SIZE=300, BLOCK=10, N=5, input2.txt"
    echo "4 | ./$EXE RAID5 300 10 5 input_and_update_files/input2.txt input_and_update_files/update2.txt backup.txt allData.txt"
    echo "4" | ./$EXE RAID5 300 10 5 input_and_update_files/input2.txt input_and_update_files/update2.txt backup.txt allData.txt
    echo ""

    echo "============================================================"
    echo "[TEST VOLUMES COMPLETE] Check '100% similarity' in each scenario."
    echo "============================================================"
    read -p "Press Enter to return to menu..."
done
