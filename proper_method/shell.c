#include "shell.h"
#include "raid.h"
#include "env.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void shell_print_help() {
    printf("\n--- Διαθέσιμες Εντολές ---\n");
    printf("INIT <level> <disks> : Αρχικοποίηση RAID (π.χ. INIT 5 4)\n");
    printf("WRITE <id> <data>    : Εγγραφή κειμένου σε logical block\n");
    printf("READ <id>           : Ανάγνωση logical block\n");
    printf("FAIL <disk_id>      : Προσομοίωση αποτυχίας δίσκου\n");
    printf("RECOVER <disk_id>   : Επαναφορά δίσκου\n");
    printf("STATUS              : Κατάσταση συστοιχίας\n");
    printf("RESET               : Backup υπαρχόντων δίσκων και καθαρισμός\n");
    printf("HELP                : Εμφάνιση βοήθειας\n");
    printf("EXIT                : Έξοδος\n");
    printf("---------------------------\n");
}

void shell_startup_menu() {
    int level = -1, disks = 0;
    char line[64];
    
    printf("\n--- ΡΥΘΜΙΣΗ RAID ---\n");
    printf("Ποιο RAID Level θέλετε; (0, 1, 5): ");
    if (fgets(line, sizeof(line), stdin)) sscanf(line, "%d", &level);
    
    printf("Πόσους δίσκους θα βάλετε; ");
    if (fgets(line, sizeof(line), stdin)) sscanf(line, "%d", &disks);
    
    if (raid_init(level, disks)) {
        printf("\n[OK] Το RAID %d με %d δίσκους είναι έτοιμο!\n", level, disks);
    } else {
        printf("\n[ΣΦΑΛΜΑ] Κάτι πήγε στραβά στην αρχικοποίηση.\n");
    }
}

void shell_loop() {
    char input[1024];
    int choice = 0;
    int arg1, arg2;
    byte buffer[BLOCK_SIZE];
    int current_disk_count = 0;

    printf("\nΜΕΝΟΥ ΔΙΑΧΕΙΡΙΣΗΣ\n");

    while (1) {
        printf("\n--- ΕΠΙΛΕΞΤΕ ΕΝΕΡΓΕΙΑ ---\n");
        printf("1. Γράψε δεδομένα (WRITE)\n");
        printf("2. Διάβασε δεδομένα (READ)\n");
        printf("3. Χάλασε έναν δίσκο (FAIL)\n");
        printf("4. Φτιάξε τον δίσκο (RECOVER)\n");
        printf("5. Δες την κατάσταση (STATUS)\n");
        printf("6. Νέο RAID (INIT)\n");
        printf("7. Backup & Reset\n");
        printf("8. Έξοδος (EXIT)\n");
        printf("------------------------\n");
        printf("Επιλογή [1-8]: ");

        if (!fgets(input, sizeof(input), stdin)) break;
        if (sscanf(input, "%d", &choice) != 1) continue;

        if (choice == 8) {
            raid_shutdown();
            break;
        }

        switch (choice) {
            case 1: /* WRITE */
                printf("Δώστε Logical Block ID: ");
                if (fgets(input, sizeof(input), stdin)) sscanf(input, "%d", &arg1);
                printf("Δώστε κείμενο προς εγγραφή: ");
                if (fgets(input, sizeof(input), stdin)) {
                    input[strcspn(input, "\n")] = 0;
                    memset(buffer, 0, BLOCK_SIZE);
                    strncpy((char*)buffer, input, BLOCK_SIZE);
                    if (raid_write_block(arg1, buffer)) {
                        printf("[OK] Το Block %d γράφτηκε.\n", arg1);
                    } else {
                        printf("[ERROR] Αποτυχία εγγραφής.\n");
                    }
                }
                break;

            case 2: /* READ */
                printf("Δώστε Logical Block ID προς ανάγνωση: ");
                if (fgets(input, sizeof(input), stdin)) sscanf(input, "%d", &arg1);
                memset(buffer, 0, BLOCK_SIZE);
                if (raid_read_block(arg1, buffer)) {
                    printf("\n-------------------------------\n");
                    printf("Περιεχόμενο Block %d: [%s]\n", arg1, buffer);
                    printf("-------------------------------\n");
                } else {
                    printf("[ERROR] Αποτυχία ανάγνωσης.\n");
                }
                break;

            case 3: /* FAIL */
                printf("Δώστε Disk ID για προσομοίωση βλάβης: ");
                if (fgets(input, sizeof(input), stdin)) sscanf(input, "%d", &arg1);
                raid_fail_disk(arg1);
                printf("[!] Ο δίσκος %d είναι πλέον εκτός λειτουργίας.\n", arg1);
                break;

            case 4: /* RECOVER */
                printf("Δώστε Disk ID προς ανάκτηση: ");
                if (fgets(input, sizeof(input), stdin)) sscanf(input, "%d", &arg1);
                if (raid_recover_disk(arg1)) {
                    printf("[OK] Ο δίσκος %d επανήλθε.\n", arg1);
                } else {
                    printf("[ERROR] Αποτυχία ανάκτησης.\n");
                }
                break;

            case 5: /* STATUS */
                raid_print_status();
                break;

            case 6: /* INIT */
                printf("Επιλέξτε RAID Level (0, 1, 5): ");
                if (fgets(input, sizeof(input), stdin)) sscanf(input, "%d", &arg1);
                printf("Επιλέξτε αριθμό δίσκων: ");
                if (fgets(input, sizeof(input), stdin)) sscanf(input, "%d", &arg2);
                if (raid_init(arg1, arg2)) {
                    current_disk_count = arg2;
                    printf("[OK] Νέα αρχικοποίηση RAID %d.\n", arg1);
                } else {
                    printf("[ERROR] Σφάλμα αρχικοποίησης.\n");
                }
                break;

            case 7: /* RESET */
                printf("Είστε σίγουροι ότι θέλετε backup & reset; (y/n): ");
                if (fgets(input, sizeof(input), stdin) && (input[0] == 'y' || input[0] == 'Y')) {
                    /* Χρησιμοποιούμε τη μεταβλητή arg2 από το τελευταίο INIT αν υπάρχει */
                    raid_shutdown();
                    env_reset_to_backups(4); /* Default 4 αν δεν ξέρουμε */
                    printf("[OK] Οι δίσκοι μετακινήθηκαν στο '%s'.\n", BACKUP_DIR);
                }
                break;

            default:
                printf("Λανθασμένη επιλογή.\n");
                break;
        }
    }
}
