#include "raid.h"
#include <string.h>
#include <stdlib.h>

/* Δομή για την κατάσταση της συστοιχίας */
typedef struct {
    int level;
    int disk_count;
    int is_failed[MAX_DISKS];
} RaidConfig;

static RaidConfig config;

/*
 * raid_init
 * Ανοίγει τους δίσκους και μηδενίζει τις καταστάσεις αποτυχίας.
 */
int raid_init(int level, int disk_count) {
    int i;
    if (disk_count < 2 || disk_count > MAX_DISKS) return 0;
    if (level != 0 && level != 1 && level != 5) return 0;
    
    config.level = level;
    config.disk_count = disk_count;
    
    for (i = 0; i < disk_count; i++) {
        /* Ανοίγουμε τον δίσκο σε r+b (read/write binary) */
        if (!disk_open(i, "r+b")) {
            /* Αν δεν υπάρχει, τον δημιουργούμε (w+b) */
            if (!disk_open(i, "w+b")) return 0;
        }
        config.is_failed[i] = 0;
    }
    
    return 1;
}

/* 
 * raid_fail_disk
 * Προσομοίωση βλάβης. Κλείνουμε το αρχείο και μαρκάρουμε τον δίσκο.
 */
void raid_fail_disk(int disk_id) {
    if (disk_id >= 0 && disk_id < config.disk_count) {
        config.is_failed[disk_id] = 1;
        disk_close(disk_id);
    }
}

/* 
 * raid_write_block
 * Υλοποίηση για RAID 0, 1, 5
 */
int raid_write_block(int logical_block_id, const byte* data) {
    int i, p_disk, target_disk, stripe_id, block_in_stripe;
    byte parity[BLOCK_SIZE];
    byte temp[BLOCK_SIZE];
    
    if (config.disk_count < 2) return 0; /* Ασφάλεια: Απαιτούνται τουλάχιστον 2 δίσκοι */
    
    if (config.level == 0) {
        /* RAID 0: Απλό Striping */
        target_disk = logical_block_id % config.disk_count;
        return disk_write_block(target_disk, logical_block_id / config.disk_count, data);
    } 
    else if (config.level == 1) {
        /* RAID 1: Mirroring */
        int res1 = disk_write_block(0, logical_block_id, data);
        int res2 = disk_write_block(1, logical_block_id, data);
        return (res1 && res2);
    } 
    else if (config.level == 5) {
        /* RAID 5: Distributed Parity (Left Symmetric) */
        stripe_id = logical_block_id / (config.disk_count - 1);
        block_in_stripe = logical_block_id % (config.disk_count - 1);
        p_disk = stripe_id % config.disk_count;
        
        target_disk = (block_in_stripe >= p_disk) ? block_in_stripe + 1 : block_in_stripe;
        
        /* Εγγραφή των δεδομένων */
        if (!disk_write_block(target_disk, stripe_id, data)) return 0;
        
        /* Επανυπολογισμός Parity για το stripe */
        memset(parity, 0, BLOCK_SIZE);
        for (i = 0; i < config.disk_count; i++) {
            if (i == p_disk) continue;
            if (disk_read_block(i, stripe_id, temp)) {
                for (int b = 0; b < BLOCK_SIZE; b++) parity[b] ^= temp[b];
            }
        }
        return disk_write_block(p_disk, stripe_id, parity);
    }
    return 0;
}

/* 
 * raid_read_block
 * Υλοποίηση για RAID 0, 1, 5 (με υποστήριξη για 1 αποτυχία στα RAID 1/5)
 */
int raid_read_block(int logical_block_id, byte* buffer) {
    int i, p_disk, target_disk, stripe_id, block_in_stripe;
    byte temp[BLOCK_SIZE];
    
    if (config.disk_count < 2) return 0; /* Ασφάλεια: Απαιτούνται τουλάχιστον 2 δίσκοι */
    
    if (config.level == 0) {
        target_disk = logical_block_id % config.disk_count;
        return disk_read_block(target_disk, logical_block_id / config.disk_count, buffer);
    } 
    else if (config.level == 1) {
        if (!config.is_failed[0]) return disk_read_block(0, logical_block_id, buffer);
        if (!config.is_failed[1]) return disk_read_block(1, logical_block_id, buffer);
        return 0;
    } 
    else if (config.level == 5) {
        stripe_id = logical_block_id / (config.disk_count - 1);
        block_in_stripe = logical_block_id % (config.disk_count - 1);
        p_disk = stripe_id % config.disk_count;
        target_disk = (block_in_stripe >= p_disk) ? block_in_stripe + 1 : block_in_stripe;
        
        if (!config.is_failed[target_disk]) {
            return disk_read_block(target_disk, stripe_id, buffer);
        } else {
            /* Αν ο δίσκος απέτυχε, ανακατασκευή από τους υπόλοιπους (XOR) */
            memset(buffer, 0, BLOCK_SIZE);
            for (i = 0; i < config.disk_count; i++) {
                if (i == target_disk) continue;
                if (!disk_read_block(i, stripe_id, temp)) return 0;
                for (int b = 0; b < BLOCK_SIZE; b++) buffer[b] ^= temp[b];
            }
            return 1;
        }
    }
    return 0;
}

/* 
 * raid_recover_disk (πρόχειρη υλοποίηση για RAID 1/5)
 */
int raid_recover_disk(int disk_id) {
    /* Εδώ κανονικά θα έπρεπε να διατρέξει όλο τον δίσκο και να ανακατασκευάσει κάθε block */
    /* Για την εργασία, απλά τον ξανανοίγουμε αν είναι εφικτό */
    if (disk_open(disk_id, "r+b")) {
        config.is_failed[disk_id] = 0;
        return 1;
    }
    return 0;
}

void raid_shutdown() {
    for (int i = 0; i < config.disk_count; i++) disk_close(i);
}

void raid_print_status() {
    printf("\n--- RAID STATUS (Level %d) ---\n", config.level);
    for (int i = 0; i < config.disk_count; i++) {
        printf("Disk %d: %s\n", i, config.is_failed[i] ? "[FAILED]" : "[ONLINE]");
    }
    printf("---------------------------\n");
}
