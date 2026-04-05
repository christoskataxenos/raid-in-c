#include "disk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Static array to hold file handles for simulated disks.
 */
static FILE* disk_handles[MAX_DISKS] = {NULL};

int disk_open(const char* raidx, int disk_id, const char* mode) {
    char filename[64];
    if (disk_id < 0 || disk_id >= MAX_DISKS) return 0;
    
    /* Ensure only one handle per disk is active */
    if (disk_handles[disk_id]) {
        fclose(disk_handles[disk_id]);
    }
    
    sprintf(filename, "%s_%d.txt", raidx, disk_id);
    disk_handles[disk_id] = fopen(filename, mode);
    
    return (disk_handles[disk_id] != NULL);
}

void disk_close(int disk_id) {
    if (disk_id >= 0 && disk_id < MAX_DISKS && disk_handles[disk_id]) {
        fclose(disk_handles[disk_id]);
        disk_handles[disk_id] = NULL;
    }
}

int disk_read_bit(int disk_id, int pos) {
    if (disk_id < 0 || disk_id >= MAX_DISKS || !disk_handles[disk_id]) return EOF;
    
    if (fseek(disk_handles[disk_id], pos, SEEK_SET) != 0) return EOF;
    return fgetc(disk_handles[disk_id]);
}

int disk_write_bit(int disk_id, int pos, char bit) {
    if (disk_id < 0 || disk_id >= MAX_DISKS || !disk_handles[disk_id]) return 0;
    
    if (fseek(disk_handles[disk_id], pos, SEEK_SET) != 0) return 0;
    fputc(bit, disk_handles[disk_id]);
    
    return 1;
}

int disk_init(int disk_id, int size) {
    int i;
    if (disk_id < 0 || disk_id >= MAX_DISKS || !disk_handles[disk_id]) return 0;
    
    rewind(disk_handles[disk_id]);
    for (i = 0; i < size; i++) {
        fputc('0', disk_handles[disk_id]);
    }
    return 1;
}

void disk_remove(const char* raidx, int disk_id) {
    char filename[64];
    disk_close(disk_id);
    sprintf(filename, "%s_%d.txt", raidx, disk_id);
    remove(filename);
}
