#include "disk.h"
#include <string.h>
#include <stdlib.h>

/* Στατικός πίνακας για τα αρχεία των δίσκων */
static FILE* disk_handles[MAX_DISKS] = {NULL};

/*
 * disk_open
 */
int disk_open(const char* raidx, int disk_id, const char* mode) {
    char filename[64];
    if (disk_id < 0 || disk_id >= MAX_DISKS) return 0;
    
    sprintf(filename, "%s_%d.txt", raidx, disk_id);
    
    if (disk_handles[disk_id]) {
        fclose(disk_handles[disk_id]);
    }
    
    disk_handles[disk_id] = fopen(filename, mode);
    return (disk_handles[disk_id] != NULL);
}

/*
 * disk_close
 */
void disk_close(int disk_id) {
    if (disk_id >= 0 && disk_id < MAX_DISKS && disk_handles[disk_id]) {
        fclose(disk_handles[disk_id]);
        disk_handles[disk_id] = NULL;
    }
}

/*
 * disk_read_bit
 */
int disk_read_bit(int disk_id, int pos) {
    if (disk_id < 0 || disk_id >= MAX_DISKS || !disk_handles[disk_id]) return -1;
    fseek(disk_handles[disk_id], pos, SEEK_SET);
    return fgetc(disk_handles[disk_id]);
}

/*
 * disk_write_bit
 */
int disk_write_bit(int disk_id, int pos, char bit) {
    if (disk_id < 0 || disk_id >= MAX_DISKS || !disk_handles[disk_id]) return 0;
    fseek(disk_handles[disk_id], pos, SEEK_SET);
    fputc(bit, disk_handles[disk_id]);
    fflush(disk_handles[disk_id]);
    return 1;
}

/*
 * disk_init
 */
int disk_init(int disk_id, int size) {
    if (disk_id < 0 || disk_id >= MAX_DISKS || !disk_handles[disk_id]) return 0;
    fseek(disk_handles[disk_id], 0, SEEK_SET);
    for (int i = 0; i < size; i++) {
        fputc('0', disk_handles[disk_id]);
    }
    return 1;
}

/*
 * disk_remove
 */
void disk_remove(const char* raidx, int disk_id) {
    char filename[64];
    sprintf(filename, "%s_%d.txt", raidx, disk_id);
    disk_close(disk_id);
    remove(filename);
}
