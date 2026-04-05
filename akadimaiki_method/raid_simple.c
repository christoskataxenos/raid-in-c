#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 
 * raid_simple.c
 * Μια "εύκολη" λύση RAID (0, 1, 5) σε ΕΝΑ μόνο αρχείο.
 * Όλα μαζεμένα εδώ για να διαβάζονται εύκολα.
 */

#define MAX_DISKS 10
#define BLOCK_SIZE 512

/* --- Μεταβλητές Συστήματος --- */
int g_raid_level = -1;
int g_disk_count = 0;
FILE* g_disks[MAX_DISKS];
int g_disk_status[MAX_DISKS]; /* 1: Online, 0: Χάλασε */

/* --- Διαχείριση Αρχείων Δίσκων --- */
int disk_open(int id) {
    char filename[32];
    sprintf(filename, "disk%d.bin", id);
    g_disks[id] = fopen(filename, "rb+");
    if (!g_disks[id]) {
        g_disks[id] = fopen(filename, "wb+");
    }
    return (g_disks[id] != NULL);
}

void disk_close_all() {
    for (int i = 0; i < g_disk_count; i++) {
        if (g_disks[i]) fclose(g_disks[i]);
        g_disks[i] = NULL;
    }
}

/* --- Λογική RAID --- */
int raid_init(int level, int count) {
    if (count < 2 || count > MAX_DISKS) return 0;
    if (level == 5 && count < 3) return 0;
    
    disk_close_all();
    g_raid_level = level;
    g_disk_count = count;
    
    for (int i = 0; i < count; i++) {
        if (!disk_open(i)) return 0;
        g_disk_status[i] = 1;
    }
    return 1;
}

/* Γράφουμε ένα Block */
int raid_write(int block_id, unsigned char* data) {
    if (g_raid_level == -1) return 0;
    
    if (g_raid_level == 0) { /* RAID 0: Μοιράζουμε τα δεδομένα στους δίσκους */
        int disk = block_id % g_disk_count;
        int offset = (block_id / g_disk_count) * BLOCK_SIZE;
        if (!g_disk_status[disk]) return 0;
        fseek(g_disks[disk], offset, SEEK_SET);
        return fwrite(data, 1, BLOCK_SIZE, g_disks[disk]) == BLOCK_SIZE;
    }
    
    if (g_raid_level == 1) { /* RAID 1: Γράφουμε το ίδιο πράγμα σε όλους */
        int success = 0;
        for (int i = 0; i < g_disk_count; i++) {
            if (g_disk_status[i]) {
                fseek(g_disks[i], block_id * BLOCK_SIZE, SEEK_SET);
                if (fwrite(data, 1, BLOCK_SIZE, g_disks[i]) == BLOCK_SIZE) success = 1;
            }
        }
        return success;
    }
    
    if (g_raid_level == 5) { /* RAID 5: Με XOR Parity */
        int data_disks = g_disk_count - 1;
        int stripe = block_id / data_disks;
        int data_disk_idx = block_id % data_disks;
        int parity_disk = (g_disk_count - 1 - (stripe % g_disk_count));
        int actual_disk = (data_disk_idx < parity_disk) ? data_disk_idx : data_disk_idx + 1;
        int offset = stripe * BLOCK_SIZE;
        
        /* Γράφουμε τα δεδομένα μας */
        if (g_disk_status[actual_disk]) {
            fseek(g_disks[actual_disk], offset, SEEK_SET);
            fwrite(data, 1, BLOCK_SIZE, g_disks[actual_disk]);
        }
        
        /* Υπολογίζουμε το νέο Parity με XOR */
        unsigned char parity[BLOCK_SIZE];
        memset(parity, 0, BLOCK_SIZE);
        for (int i = 0; i < g_disk_count; i++) {
            if (i == parity_disk) continue;
            unsigned char temp[BLOCK_SIZE];
            memset(temp, 0, BLOCK_SIZE);
            fseek(g_disks[i], offset, SEEK_SET);
            fread(temp, 1, BLOCK_SIZE, g_disks[i]);
            for (int b = 0; b < BLOCK_SIZE; b++) parity[b] ^= temp[b];
        }
        if (g_disk_status[parity_disk]) {
            fseek(g_disks[parity_disk], offset, SEEK_SET);
            fwrite(parity, 1, BLOCK_SIZE, g_disks[parity_disk]);
        }
        return 1;
    }
    return 0;
}

/* Διαβάζουμε ένα Block */
int raid_read(int block_id, unsigned char* data) {
    if (g_raid_level == -1) return 0;
    
    if (g_raid_level == 0) {
        int disk = block_id % g_disk_count;
        int offset = (block_id / g_disk_count) * BLOCK_SIZE;
        if (!g_disk_status[disk]) return 0;
        fseek(g_disks[disk], offset, SEEK_SET);
        return fread(data, 1, BLOCK_SIZE, g_disks[disk]) == BLOCK_SIZE;
    }
    
    if (g_raid_level == 1) {
        for (int i = 0; i < g_disk_count; i++) {
            if (g_disk_status[i]) {
                fseek(g_disks[i], block_id * BLOCK_SIZE, SEEK_SET);
                return fread(data, 1, BLOCK_SIZE, g_disks[i]) == BLOCK_SIZE;
            }
        }
        return 0;
    }
    
    if (g_raid_level == 5) {
        int data_disks = g_disk_count - 1;
        int stripe = block_id / data_disks;
        int data_disk_idx = block_id % data_disks;
        int parity_disk = (g_disk_count - 1 - (stripe % g_disk_count));
        int actual_disk = (data_disk_idx < parity_disk) ? data_disk_idx : data_disk_idx + 1;
        int offset = stripe * BLOCK_SIZE;
        
        if (g_disk_status[actual_disk]) {
            fseek(g_disks[actual_disk], offset, SEEK_SET);
            return fread(data, 1, BLOCK_SIZE, g_disks[actual_disk]) == BLOCK_SIZE;
        } else {
            /* Αν λείπει ο δίσκος, το ξαναφτιάχνουμε από τα υπόλοιπα (XOR) */
            int failed_counts = 0;
            for(int i=0; i<g_disk_count; i++) if(!g_disk_status[i]) failed_counts++;
            if (failed_counts > 1) return 0;
            
            memset(data, 0, BLOCK_SIZE);
            for (int i = 0; i < g_disk_count; i++) {
                if (i == actual_disk) continue;
                unsigned char temp[BLOCK_SIZE];
                fseek(g_disks[i], offset, SEEK_SET);
                fread(temp, 1, BLOCK_SIZE, g_disks[i]);
                for (int b = 0; b < BLOCK_SIZE; b++) data[b] ^= temp[b];
            }
            return 1;
        }
    }
    return 0;
}

/* --- Το Μενού --- */
void menu() {
    int choice, arg1, arg2;
    unsigned char buffer[BLOCK_SIZE];
    char line[1024];

    while(1) {
        printf("\n--- ΕΞΟΜΟΙΩΤΗΣ RAID ---\n");
        printf("1. Φτιάξε RAID (INIT)\n2. Γράψε (WRITE)\n3. Διάβασε (READ)\n4. Χάλασε δίσκο (FAIL)\n5. Φτιάξε δίσκο (RECOVER)\n6. Κατάσταση\n7. Έξοδος\n");
        printf("Επιλογή: ");
        if (!fgets(line, sizeof(line), stdin)) break;
        sscanf(line, "%d", &choice);

        if (choice == 7) break;
        
        switch(choice) {
            case 1:
                printf("Ποιο Level; (0,1,5): "); fgets(line, sizeof(line), stdin); sscanf(line, "%d", &arg1);
                printf("Πόσοι δίσκοι; "); fgets(line, sizeof(line), stdin); sscanf(line, "%d", &arg2);
                if (raid_init(arg1, arg2)) printf("Έτοιμο!\n"); else printf("Σφάλμα!\n");
                break;
            case 2:
                printf("Ποιο Block; "); fgets(line, sizeof(line), stdin); sscanf(line, "%d", &arg1);
                printf("Τι να γράψω; "); fgets(line, sizeof(line), stdin);
                line[strcspn(line, "\n")] = 0;
                memset(buffer, 0, BLOCK_SIZE);
                strncpy((char*)buffer, line, BLOCK_SIZE);
                if (raid_write(arg1, buffer)) printf("Έγινε!\n"); else printf("Απέτυχε!\n");
                break;
            case 3:
                printf("Ποιο Block; "); fgets(line, sizeof(line), stdin); sscanf(line, "%d", &arg1);
                if (raid_read(arg1, buffer)) printf("Δεδομένα: [%s]\n", buffer); else printf("Πρόβλημα!\n");
                break;
            case 4:
                printf("Ποιος δίσκος χάλασε; "); fgets(line, sizeof(line), stdin); sscanf(line, "%d", &arg1);
                g_disk_status[arg1] = 0;
                printf("Disk %d Failed!\n", arg1);
                break;
            case 5:
                printf("Ποιον δίσκο φτιάξαμε; "); fgets(line, sizeof(line), stdin); sscanf(line, "%d", &arg1);
                g_disk_status[arg1] = 1;
                printf("Disk %d Online!\n", arg1);
                break;
            case 6:
                printf("RAID Level: %d, Δίσκοι: %d\n", g_raid_level, g_disk_count);
                for(int i=0; i<g_disk_count; i++) printf("Δίσκος %d: %s\n", i, g_disk_status[i] ? "ONLINE" : "FAILED");
                break;
            default: printf("Λάθος επιλογή!\n");
        }
    }
}

int main() {
    menu();
    disk_close_all();
    return 0;
}
