#!/bin/bash
EXE="akadimaiki_method/ergasia1.exe"

# Clear the screen
clear
echo "============================================================"
echo "[RAID SIMULATOR TEST SUITE - WSL/Linux]"
echo "============================================================"
echo ""

# Ensure the executable exists
if [ ! -f "$EXE" ]; then
    echo "[ERROR] $EXE not found. Please compile it first:"
    echo "cd akadimaiki_method && gcc raid_simple.c -o ergasia1.exe"
    exit 1
fi

# SCENARIO 1: RAID 1 (Mirroring)
clear
echo "--- [SCENARIO 1: RAID 1] ---"
echo "RAID 1, SIZE=30, BLOCK=5, N=2, input1.txt"
echo "echo 1 | ./$EXE RAID1 30 5 2 input_and_update_files/input1.txt input_and_update_files/update1.txt backup.txt allData.txt"
echo 1 | ./"$EXE" RAID1 30 5 2 input_and_update_files/input1.txt input_and_update_files/update1.txt backup.txt allData.txt
echo ""
read -p "[!] Πατήστε [Enter] για να πάμε στο επόμενο σενάριο..." null

# SCENARIO 2: RAID 4 (Fixed Parity)
clear
echo "--- [SCENARIO 2: RAID 4] ---"
echo "RAID 4, SIZE=25, BLOCK=5, N=3, input1.txt"
echo "echo 1 | ./$EXE RAID4 25 5 3 input_and_update_files/input1.txt input_and_update_files/update1.txt backup.txt allData.txt"
echo 1 | ./"$EXE" RAID4 25 5 3 input_and_update_files/input1.txt input_and_update_files/update1.txt backup.txt allData.txt
echo ""
read -p "[!] Πατήστε [Enter] για να πάμε στο επόμενο σενάριο..." null

# SCENARIO 3: RAID 5 (Distributed Parity)
clear
echo "--- [SCENARIO 3: RAID 5] ---"
echo "RAID 5, SIZE=20, BLOCK=5, N=5, input1.txt"
echo "echo 2 | ./$EXE RAID5 20 5 5 input_and_update_files/input1.txt input_and_update_files/update1.txt backup.txt allData.txt"
echo 2 | ./"$EXE" RAID5 20 5 5 input_and_update_files/input1.txt input_and_update_files/update1.txt backup.txt allData.txt
echo ""
read -p "[!] Πατήστε [Enter] για να πάμε στο επόμενο σενάριο..." null

# SCENARIO 4: RAID 1E (Enhanced Mirroring)
clear
echo "--- [SCENARIO 4: RAID 1E] ---"
echo "RAID 1E, SIZE=25, BLOCK=5, N=3, input1.txt"
echo "echo 0 | ./$EXE RAID1E 25 5 3 input_and_update_files/input1.txt input_and_update_files/update1.txt backup.txt allData.txt"
echo 0 | ./"$EXE" RAID1E 25 5 3 input_and_update_files/input1.txt input_and_update_files/update1.txt backup.txt allData.txt
echo ""
read -p "[!] Πατήστε [Enter] για να πάμε στο επόμενο σενάριο..." null

# SCENARIO 5: LARGE DATA TEST
clear
echo "--- [SCENARIO 5: LARGE DATA] ---"
echo "RAID 5, SIZE=300, BLOCK=10, N=5, input2.txt"
echo "echo 4 | ./$EXE RAID5 300 10 5 input_and_update_files/input2.txt input_and_update_files/update2.txt backup.txt allData.txt"
echo 4 | ./"$EXE" RAID5 300 10 5 input_and_update_files/input2.txt input_and_update_files/update2.txt backup.txt allData.txt
echo ""

echo "============================================================"
echo "[TESTS COMPLETE] Check '100% similarity' in each scenario."
echo "============================================================"
