# RAID Simulator - Proper Method (Modular)

This directory contains the modular implementation of the RAID simulator, separated into functional components for maximum clarity and structural integrity.

---

## 📂 File Structure
-   **`main.c`**: Orchestration of the 8-step simulation workflow (CLI Parsing, Flow Control).
-   **`raid.c / raid.h`**: RAID Logic implementation (1, 1E, 4, 5) and XOR Parity calculation.
-   **`disk.c / disk.h`**: Low-level disk file management and Bit-Character I/O.
-   **`compile.bat`**: Automated compilation script for Windows.

---

## 🛠️ Compilation

### 💻 Windows (PowerShell)
```powershell
gcc main.c raid.c disk.c -o ergasia1.exe
```

### 🐧 Linux / WSL2
```bash
gcc main.c raid.c disk.c -o ergasia1.exe
```

---

## 🚀 Execution

### 💻 Windows
`./ergasia1.exe RAIDX SIZE BLOCKSIZE N inputFile updates backup allData`

**Example**:
`./ergasia1.exe RAID5 20 5 4 ../input_and_update_files/input2.txt ../input_and_update_files/update2.txt ../backup.txt ../allData.txt`

### 🐧 Linux / WSL2
`./ergasia1.exe RAIDX SIZE BLOCKSIZE N inputFile updates backup allData`

---

## 🔎 Features
-   **RAID 1, 1E, 4, 5 Recovery**: Full support for data reconstruction.
-   **Bit-Character Simulation**: Disks (`RAIDX_no.txt`) store '0'/'1' for easy bitwise verification.
-   **Dual-Mode Input**: Supports both CLI arguments and Interactive user prompts.

---

## ⚠️ Troubleshooting
If Windows blocks the `.exe` with a **"Device Guard policy"** message, please switch to **WSL2** or a Linux terminal for execution.
