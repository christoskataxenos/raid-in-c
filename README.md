# RAID Simulator Project (C)

A comprehensive RAID level simulation system (1, 1E, 4, 5) that strictly follows academic specifications for bit-char level simulation. Supporting both single-file and modular implementations.

---

## 📂 Project Structure

-   **`akadimaiki_method/`**: Clean single-file implementation (`raid_simple.c`), ideal for academic submission.
-   **`proper_method/`**: Professional modular implementation with separate files (`main.c`, `raid.c`, `disk.c`) for superior architecture.
-   **`input_and_update_files/`**: Test data containing initial bitstreams and delta updates.

---

## 🚀 Smart Testing & Auto-Compile

The project features a unified test suite that handles **Automatic Compilation** and allows you to **Select the Version** you want to test.

### 💻 Windows
```powershell
.\run_all_tests.bat
```
1. Run the script.
2. Select `1` (Academic) or `2` (Proper).
3. The script will automatically run `gcc` and start the 4 test scenarios.

### 🐧 Linux / WSL2
```bash
chmod +x run_all_tests.sh
./run_all_tests.sh
```
1. Run the bash script.
2. Select your version.
3. Review the automated bitwise reconstruction results.

---

## 📈 8-Step Academic Workflow

Both implementations strictly adhere to the required sequence:
1.  **Init**: Disk allocation and zero-initialization.
2.  **Load**: Bitwise data distribution from `inputFile`.
3.  **Backup**: Persistence of the original bitstream for verification.
4.  **Update**: Dynamic bit-flipping from `updates.txt` with parity synchronization.
5.  **Fail**: Simulation of a hardware failure (Disk Removal).
6.  **Recover**: Data restoration using Parity/Mirror logic.
7.  **Reconstruct**: Global reassembly into a single `allData.txt` file.
8.  **Compare**: Automated Audit for 100% bit-perfect similarity.

---

## ⚠️ Important Notes

> [!IMPORTANT]
> **Dual-Mode Input**: Both versions support **Command-Line Arguments** (for automated testing) and an **Interactive Menu** (if run directly).

> [!WARNING]
> **Device Guard Policy**: If Windows blocks `.exe` files, use **WSL2** to compile and run. The automation scripts will work identically in a Linux environment.
