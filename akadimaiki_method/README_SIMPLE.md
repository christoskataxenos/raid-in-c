# RAID Simulator - Simple Method (Single File)

This folder contains the refined implementation of the RAID simulator in a single source file (`raid_simple.c`). It is designed for academic clarity while maintaining professional modular logic under the hood.

---

## 🛠️ Performance & Features
-   **RAID 1, 1E, 4, 5**: Full bit-level support.
-   **Dual-Mode Input**: Supports CLI arguments for automation or Interactive Prompts for manual use.
-   **ANSI C Clean**: Compatible with a wide range of C compilers.
-   **English Localization**: All UI strings and source comments are in English.

---

## 🚀 Compilation & Execution

### 💻 Manual Compilation
```powershell
gcc raid_simple.c -o ergasia1.exe
```

### 🚀 Smart Execution
The easiest way to test this version is via the **root** testing script:
```powershell
cd ..
.\run_all_tests.bat
# Then Select Option [1]
```

---

## 🔍 Simulation Logic
The program follows the 8-step academic sequence: **Initialization -> Loading -> Backup -> Updates -> Failure -> Recovery -> Reconstruction -> Verification.**

---

## ⚠️ Compatibility
If you encounter **WDAC / Device Guard** blocks on Windows, compile and run the source using **WSL2 (Windows Subsystem for Linux)**.
