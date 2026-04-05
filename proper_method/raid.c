#include "raid.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int raid_init(const char* raidx, int n, int size) {
    int i;
    for (i = 0; i < n; i++) {
        if (!disk_open(raidx, i, "w+")) return 0;
        disk_init(i, size);
    }
    return 1;
}

int raid_write_bit(const char* raidx, int pos, int n, int blocksize, char bit, int is_update) {
    int bit_val = bit - '0';
    
    if (strcmp(raidx, "RAID1") == 0) {
        int i;
        for (i = 0; i < n; i++) disk_write_bit(i, pos, bit);
    } 
    else if (strcmp(raidx, "RAID1E") == 0) {
        int d = pos % n;
        int m = (d + 1) % n;
        disk_write_bit(d, pos, bit);
        disk_write_bit(m, pos, bit);
    }
    else if (strcmp(raidx, "RAID4") == 0) {
        int data_disks = n - 1;
        int target = (pos / blocksize) % data_disks;
        int stripe = pos / (blocksize * data_disks);
        int local = stripe * blocksize + (pos % blocksize);
        int parity_disk = n - 1;
        
        int old_bit_val = disk_read_bit(target, local) - '0';
        int old_p_val = disk_read_bit(parity_disk, local) - '0';
        int new_p_val = old_p_val ^ old_bit_val ^ bit_val;
        
        disk_write_bit(target, local, bit);
        disk_write_bit(parity_disk, local, new_p_val + '0');
    }
    else if (strcmp(raidx, "RAID5") == 0) {
        int data_disks = n - 1;
        int stripe = pos / (blocksize * data_disks);
        int block_in_stripe = (pos % (blocksize * data_disks)) / blocksize;
        int p_disk = stripe % n;
        int target = (block_in_stripe >= p_disk) ? block_in_stripe + 1 : block_in_stripe;
        int local = stripe * blocksize + (pos % blocksize);
        
        int old_bit_val = disk_read_bit(target, local) - '0';
        int old_p_val = disk_read_bit(p_disk, local) - '0';
        int new_p_val = old_p_val ^ old_bit_val ^ bit_val;
        
        disk_write_bit(target, local, bit);
        disk_write_bit(p_disk, local, new_p_val + '0');
    }
    return 1;
}

char raid_read_bit(const char* raidx, int pos, int n, int blocksize) {
    if (strcmp(raidx, "RAID1") == 0) return disk_read_bit(0, pos);
    if (strcmp(raidx, "RAID1E") == 0) return disk_read_bit(pos % n, pos);
    
    if (strcmp(raidx, "RAID4") == 0) {
        int data_disks = n - 1;
        int target = (pos / blocksize) % data_disks;
        int stripe = pos / (blocksize * data_disks);
        int local = stripe * blocksize + (pos % blocksize);
        return disk_read_bit(target, local);
    }
    
    if (strcmp(raidx, "RAID5") == 0) {
        int data_disks = n - 1;
        int stripe = pos / (blocksize * data_disks);
        int block_in_stripe = (pos % (blocksize * data_disks)) / blocksize;
        int p_disk = stripe % n;
        int target = (block_in_stripe >= p_disk) ? block_in_stripe + 1 : block_in_stripe;
        int local = stripe * blocksize + (pos % blocksize);
        return disk_read_bit(target, local);
    }
    return '0';
}

void raid_recover(const char* raidx, int failed_id, int n, int size) {
    int i;
    /* Re-open the failed disk in w+ mode for reconstruction */
    disk_open(raidx, failed_id, "w+");
    
    for (i = 0; i < size; i++) {
        if (strcmp(raidx, "RAID1") == 0) {
            int src = (failed_id == 0) ? 1 : 0;
            disk_write_bit(failed_id, i, disk_read_bit(src, i));
        }
        else if (strcmp(raidx, "RAID1E") == 0) {
            int src = (failed_id + 1) % n;
            disk_write_bit(failed_id, i, disk_read_bit(src, i));
        }
        else { /* RAID 4/5 XOR Reconstruction */
            int recovered = 0;
            int k;
            for (k = 0; k < n; k++) {
                if (k == failed_id) continue;
                recovered ^= (disk_read_bit(k, i) - '0');
            }
            disk_write_bit(failed_id, i, recovered + '0');
        }
    }
}

void raid_compare(const char* file1, const char* file2) {
    FILE* f1 = fopen(file1, "r");
    FILE* f2 = fopen(file2, "r");
    int c1, c2, match = 1;
    if (!f1 || !f2) { printf("[ERROR] Comparison data stream failure.\n"); return; }
    
    while (1) {
        c1 = fgetc(f1); c2 = fgetc(f2);
        if (c1 != c2) { match = 0; break; }
        if (c1 == EOF) break;
    }
    
    if (match) printf(">> 100%% similarity confirmed.\n");
    else printf(">> Data mismatch detected!\n");
    
    fclose(f1); fclose(f2);
}

void raid_shutdown() {
    int i;
    for (i = 0; i < MAX_DISKS; i++) disk_close(i);
}
