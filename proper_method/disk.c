#include "disk.h"
#include <string.h>

/* Στατικός πίνακας για τα αρχεία των δίσκων */
static FILE* disk_handles[MAX_DISKS] = {NULL};

/*
 * disk_is_ready
 * Σύντομος έλεγχος αν ο disk_id είναι έγκυρος και ο δίσκος ανοιχτός.
 */
int disk_is_ready(int disk_id) {
    if (disk_id < 0 || disk_id >= MAX_DISKS) return 0;
    return (disk_handles[disk_id] != NULL);
}

/*
 * disk_open
 * Δημιουργεί το όνομα αρχείου (π.χ. disk0.bin) και το ανοίγει.
 */
int disk_open(int disk_id, const char* mode) {
    char filename[32];
    if (disk_id < 0 || disk_id >= MAX_DISKS) return 0;
    
    /* 
     * Χρήση sprintf για συμβατότητα με παλαιότερους compilers (Windows 2000). 
     * Το filename[32] είναι υπεραρκετό για το "disk%d.bin".
     */
    sprintf(filename, "disk%d.bin", disk_id);
    
    /* Αν ο δίσκος ήταν ήδη ανοιχτός, τον κλείνουμε */
    if (disk_handles[disk_id]) {
        fclose(disk_handles[disk_id]);
    }
    
    disk_handles[disk_id] = fopen(filename, mode);
    
    return (disk_handles[disk_id] != NULL);
}

/*
 * disk_close
 * Κλείνει τον δίσκο αν είναι ανοιχτός.
 */
void disk_close(int disk_id) {
    if (disk_id >= 0 && disk_id < MAX_DISKS && disk_handles[disk_id]) {
        fclose(disk_handles[disk_id]);
        disk_handles[disk_id] = NULL;
    }
}

/*
 * disk_read_block
 * Μετακινεί τον κέρσορα (fseek) στη σωστή θέση και διαβάζει 512 bytes.
 */
int disk_read_block(int disk_id, int block_id, byte* buffer) {
    if (!disk_is_ready(disk_id)) return 0;
    
    /* fseek στην αρχή του επιθυμητού block */
    if (fseek(disk_handles[disk_id], (long)block_id * BLOCK_SIZE, SEEK_SET) != 0) {
        return 0;
    }
    
    return (fread(buffer, 1, BLOCK_SIZE, disk_handles[disk_id]) == BLOCK_SIZE);
}

/*
 * disk_write_block
 * Μετακινεί τον κέρσορα και γράφει 512 bytes.
 */
int disk_write_block(int disk_id, int block_id, const byte* buffer) {
    if (!disk_is_ready(disk_id)) return 0;
    
    /* fseek στην αρχή του επιθυμητού block */
    if (fseek(disk_handles[disk_id], (long)block_id * BLOCK_SIZE, SEEK_SET) != 0) {
        return 0;
    }
    
    if (fwrite(buffer, 1, BLOCK_SIZE, disk_handles[disk_id]) != BLOCK_SIZE) {
        return 0;
    }
    
    /* fflush για να είμαστε σίγουροι ότι τα δεδομένα γράφτηκαν στο αρχείο */
    fflush(disk_handles[disk_id]);
    
    return 1;
}
