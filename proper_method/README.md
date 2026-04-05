# RAID Simulator - Proper Method (Modular)

Αυτός ο φάκελος περιέχει την σπονδυλωτή (Modular) υλοποίηση του προσομοιωτή RAID, χωρισμένη σε λειτουργικές ενότητες για μέγιστη καθαρότητα και δομή.

---

## 📂 Αρχεία
-   **`main.c`**: Ορχήστρα των 8 βημάτων (CLI Parsing, Flow Control).
-   **`raid.c / raid.h`**: Λογική RAID (1, 1E, 4, 5) και XOR Parity Calculation.
-   **`disk.c / disk.h`**: Διαχείριση αρχείων δίσκων και Bit-Char I/O.
-   **`compile.bat`**: Αυτοματοποιημένη μεταγλώττιση για Windows.

---

## 🛠️ Μεταγλώττιση (Compilation)

### 💻 Windows (PowerShell)
```powershell
gcc main.c raid.c disk.c -o ergasia1.exe
```

### 🐧 Linux / WSL2
```bash
gcc main.c raid.c disk.c -o ergasia1.exe
```

---

## 🚀 Εκτέλεση (Execution)

### 💻 Windows
`./ergasia1.exe RAIDX SIZE BLOCKSIZE N inputFile updates backup allData`

**Παράδειγμα**:
`./ergasia1.exe RAID5 20 5 4 ../input_and_update_files/input2.txt ../input_and_update_files/update2.txt ../backup.txt ../allData.txt`

### 🐧 Linux / WSL2
`./ergasia1.exe RAIDX SIZE BLOCKSIZE N inputFile updates backup allData`

---

## 🔎 Χαρακτηριστικά
-   **RAID 1, 1E, 4, 5**: Πλήρης υποστήριξη.
-   **Bit-Character Simulation**: Οι δίσκοι (`RAIDX_no.txt`) αποθηκεύουν '0'/'1' για εύκολη επαλήθευση.
-   **Advanced Logging**: Αναλυτική περιγραφή κάθε XOR πράξης στην κονσόλα.

---

## ⚠️ Προσοχή
Αν τα Windows μπλοκάρουν το `.exe` με μήνυμα **"Device Guard policy"**, μεταφερθείτε στο **WSL2** για την εκτέλεση.
