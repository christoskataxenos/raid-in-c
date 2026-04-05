#include "raid.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
 * raid_init
 */
int raid_init(const char* raidx, int n, int size) {
    for (int i = 0; i < n; i++) {
        if (!disk_open(raidx, i, "w+")) return 0;
        disk_init(i, size);
    }
    return 1;
}

/*
 * raid_write_bit
 */
int raid_write_bit(const char* raidx, int pos, int n, int blocksize, char bit, int is_update) {
    double cap_multiplier = (strcmp(raidx, "RAID_1") == 0) ? 1.0 : 
                            (strcmp(raidx, "RAID_1E") == 0) ? n / 2.0 : (n - 1.0);
    
    int bit_val = bit - '0';
    
    if (strcmp(raidx, "RAID1") == 0) {
        for (int i = 0; i < n; i++) disk_write_bit(i, pos, bit);
        if (is_update) printf(" - Bit %d: [RAID1] Mirrored to all disks. OK\n", pos);
    } 
    else if (strcmp(raidx, "RAID1E") == 0) {
        int d = pos % n;
        int m = (d + 1) % n;
        disk_write_bit(d, pos, bit);
        disk_write_bit(m, pos, bit);
        if (is_update) printf(" - Bit %d: [RAID1E] Mirrored to Disk %d and %d. OK\n", pos, d, m);
    }
    else if (strcmp(raidx, "RAID4") == 0) {
        int target = (pos / blocksize) % (n-1);
        int stripe = pos / (blocksize * (n-1));
        int local = stripe * blocksize + (pos % blocksize);
        int parity_disk = n - 1;
        
        int old_bit = disk_read_bit(target, local) - '0';
        int old_p = disk_read_bit(parity_disk, local) - '0';
        int new_p = old_p ^ old_bit ^ bit_val;
        
        disk_write_bit(target, local, bit);
        disk_write_bit(parity_disk, local, new_p + '0');
        
        if (is_update) {
            printf(" - Bit %d: [RAID4] Disk %d changed %d->%d. Parity (Disk %d) %d->%d. (XOR Logic: %d ^ %d ^ %d = %d)\n", 
                   pos, target, old_bit, bit_val, parity_disk, old_p, new_p, old_p, old_bit, bit_val, new_p);
        }
    }
    else if (strcmp(raidx, "RAID5") == 0) {
        int stripe = pos / (blocksize * (n-1));
        int b_in_s = (pos % (blocksize * (n-1))) / blocksize;
        int p_disk = stripe % n;
        int target = (b_in_s >= p_disk) ? b_in_s + 1 : b_in_s;
        int local = stripe * blocksize + (pos % blocksize);
        
        int old_bit = disk_read_bit(target, local) - '0';
        int old_p = disk_read_bit(p_disk, local) - '0';
        int new_p = old_p ^ old_bit ^ bit_val;
        
        disk_write_bit(target, local, bit);
        disk_write_bit(p_disk, local, new_p + '0');
        
        if (is_update) {
            printf(" - Bit %d: [RAID5] Disk %d changed %d->%d. Parity (Disk %d) %d->%d. (XOR Logic: %d ^ %d ^ %d = %d)\n", 
                   pos, target, old_bit, bit_val, p_disk, old_p, new_p, old_p, old_bit, bit_val, new_p);
        }
    }
    return 1;
}

/*
 * raid_read_bit
 */
char raid_read_bit(const char* raidx, int pos, int n, int blocksize) {
    if (strcmp(raidx, "RAID1") == 0) return disk_read_bit(0, pos);
    if (strcmp(raidx, "RAID1E") == 0) return disk_read_bit(pos % n, pos);
    
    if (strcmp(raidx, "RAID4") == 0) {
        int target = (pos / blocksize) % (n-1);
        int stripe = pos / (blocksize * (n-1));
        int local = stripe * blocksize + (pos % blocksize);
        return disk_read_bit(target, local);
    }
    
    if (strcmp(raidx, "RAID5") == 0) {
        int stripe = pos / (blocksize * (n-1));
        int b_in_s = (pos % (blocksize * (n-1))) / blocksize;
        int p_disk = stripe % n;
        int target = (b_in_s >= p_disk) ? b_in_s + 1 : b_in_s;
        int local = stripe * blocksize + (pos % blocksize);
        return disk_read_bit(target, local);
    }
    return '0';
}

/*
 * raid_recover
 */
void raid_recover(const char* raidx, int failed_id, int n, int size) {
    disk_open(raidx, failed_id, "w+");
    for (int i = 0; i < size; i++) {
        if (strcmp(raidx, "RAID1") == 0) {
            int src = (failed_id == 0) ? 1 : 0;
            disk_write_bit(failed_id, i, disk_read_bit(src, i));
        }
        else if (strcmp(raidx, "RAID1E") == 0) {
            int src = (failed_id + 1) % n;
            int bit = disk_read_bit(src, i);
            if (bit == EOF) bit = disk_read_bit((failed_id + n - 1) % n, i);
            disk_write_bit(failed_id, i, bit);
        }
        else { /* RAID 4/5 XOR */
            int recovered = 0;
            for (int k = 0; k < n; k++) {
                if (k == failed_id) continue;
                recovered ^= (disk_read_bit(k, i) - '0');
            }
            disk_write_bit(failed_id, i, recovered + '0');
        }
    }
}

/*
 * raid_compare
 */
void raid_compare(const char* file1, const char* file2) {
    FILE* f1 = fopen(file1, "r");
    FILE* f2 = fopen(file2, "r");
    int c1, c2, match = 1;
    if (!f1 || !f2) {printf("[!] Error comparing.\n"); return;}
    while ((c1 = fgetc(f1)) != EOF) {
        c2 = fgetc(f2);
        if (c1 != c2) {match = 0; break;}
    }
    if (match) printf(">> 100%% similarity\n");
    else printf(">> Files do not match!\n");
    fclose(f1); fclose(f2);
}
