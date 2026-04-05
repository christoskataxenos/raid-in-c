# RAID Simulator Project (C)

Ένα ολοκληρωμένο σύστημα προσομοίωσης επιπέδων RAID (1, 1E, 4, 5) το οποίο ακολουθεί αυστηρά τις ακαδημαϊκές προδιαγραφές για προσομοίωση σε επίπεδο bit-char.

---

## 📂 Δομή Έργου

-   **`akadimaiki_method/`**: Υλοποίηση σε ένα αρχείο (`raid_simple.c`), ιδανική για απλή παρουσίαση.
-   **`proper_method/`**: Σπονδυλωτή (Modular) υλοποίηση με ξεχωριστά αρχεία (`main.c`, `raid.c`, `disk.c`) για πιο επαγγελματική προσέγγιση.
-   **`input_and_update_files/`**: Φάκελος με δοκιμαστικά αρχεία εισόδου και αλλαγών.

---

## 🚀 Οδηγίες Μεταγλώττισης (Compilation)

### 💻 Windows (PowerShell / Command Prompt)
Προϋπόθεση: Εγκατεστημένο το GCC (π.χ. μέσω MinGW ή MSYS2).

```powershell
# Για την απλή μέθοδο:
cd akadimaiki_method
gcc raid_simple.c -o ergasia1.exe

# Για την σπονδυλωτή μέθοδο:
cd proper_method
gcc main.c raid.c disk.c -o ergasia1.exe
```

### 🐧 Linux / WSL2 (Ubuntu / Bash)
```bash
# Για την απλή μέθοδο:
cd akadimaiki_method
gcc raid_simple.c -o ergasia1.exe

# Για την σπονδυλωτή μέθοδο:
cd proper_method
gcc main.c raid.c disk.c -o ergasia1.exe
```

---

## 🛠️ Οδηγίες Εκτέλεσης (Execution)

### 💻 Windows
```powershell
./run_all_tests.bat
```

### 🐧 Linux / WSL2
```bash
chmod +x run_all_tests.sh
./run_all_tests.sh
```

---

## 📈 Ροή Εργασίας 8-Βημάτων
Το πρόγραμμα εκτελεί αυτόματα τα εξής:
1.  **Init**: Αρχικοποίηση δίσκων με '0'.
2.  **Load**: Διανομή δεδομένων από το `inputFile`.
3.  **Backup**: Δημιουργία αρχείου αναφοράς `backup.txt`.
4.  **Update**: Εφαρμογή αλλαγών από το `updates.txt` με υπολογισμό XOR.
5.  **Fail**: Προσομοίωση καταστροφής ενός δίσκου (UserInput).
6.  **Recover**: Ανάκτηση των χαμένων δεδομένων.
7.  **Reconstruct**: Επανασύνθεση όλων των δεδομένων στο `allData.txt`.
8.  **Compare**: Έλεγχος για "100% similarity" έναντι του backup.

---

## ⚠️ Αντιμετώπιση Προβλημάτων

> [!WARNING]
> **Windows Device Guard Policy**: Αν τα Windows μπλοκάρουν την εκτέλεση του `.exe` ("blocked by organization's policy"), χρησιμοποιήστε το **WSL2 (Linux Terminal)** για να κάνετε compile και να τρέξετε τα αρχεία εκεί. Το Linux δεν επηρεάζεται από αυτή την πολιτική των Windows.

> [!IMPORTANT]
> **Paths**: Σε Windows χρησιμοποιήστε backslash (`\`), ενώ σε Linux/WSL χρησιμοποιήστε forward slash (`/`). Τα παρεχόμενα scripts (`.bat` / `.sh`) το χειρίζονται αυτόματα.
