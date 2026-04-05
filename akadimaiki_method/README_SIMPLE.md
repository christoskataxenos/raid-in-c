# RAID Simulator - Simple Method (Single File)

Αυτός ο φάκελος περιέχει την υλοποίηση του προσομοιωτή RAID σε ένα αρχείο (`raid_simple.c`), σχεδιασμένο για ακαδημαϊκή παρουσίαση.

---

## 🛠️ Μεταγλώττιση (Compilation)

### 💻 Windows (PowerShell)
```powershell
gcc raid_simple.c -o ergasia1.exe
```

### 🐧 Linux / WSL2
```bash
gcc raid_simple.c -o ergasia1.exe
```

---

## 🚀 Εκτέλεση (Execution)

### 💻 Windows
`./ergasia1.exe RAIDX SIZE BLOCKSIZE N inputFile updates backup allData`

**Παράδειγμα**:
`./ergasia1.exe RAID4 20 5 3 ../input_and_update_files/input1.txt ../input_and_update_files/update1.txt ../backup.txt ../allData.txt`

### 🐧 Linux / WSL2
`./ergasia1.exe RAIDX SIZE BLOCKSIZE N inputFile updates backup allData`

---

## 🔍 Χαρακτηριστικά
-   **RAID 1, 1E, 4, 5**: Πλήρης υποστήριξη.
-   **Bit-Level Parsing**: Διαβάζει '0'/'1' χαρακτήρες.
-   **Console Logs**: Εμφανίζει λεπτομερείς υπολογισμούς XOR κατά το update και την ανάκτηση (Recovery).
-   **8-Step Sequence**: Ακολουθεί πιστά τη ροή εργασίας της άσκησης.

---

## ⚠️ Προσοχή
Αν τα Windows μπλοκάρουν το `.exe` με μήνυμα **"Device Guard policy"**, μεταφερθείτε στο **WSL2** για την εκτέλεση.
