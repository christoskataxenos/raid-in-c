#ifndef RAID_H
#define RAID_H

#include "disk.h"

/* RAID Levels Supported */
#define RAID_1  1
#define RAID_1E 2
#define RAID_4  4
#define RAID_5  5

/**
 * Initialize all N disks for a given RAID level.
 */
int raid_init(const char* raidx, int n, int size);

/**
 * Write a data bit to the RAID system.
 * raidx: RAID level name
 * pos: Data bit position
 * n: Number of disks
 * blocksize: Stripe block size
 * bit: Character '0' or '1'
 * is_update: Flag to enable verbose output for bit updates
 */
int raid_write_bit(const char* raidx, int pos, int n, int blocksize, char bit, int is_update);

/**
 * Read a specific data bit from the RAID system.
 */
char raid_read_bit(const char* raidx, int pos, int n, int blocksize);

/**
 * Recover a failed disk using the survivors.
 */
void raid_recover(const char* raidx, int failed_id, int n, int size);

/**
 * Compare two files for 100% similarity.
 */
void raid_compare(const char* file1, const char* file2);

/**
 * Close all active disk handles.
 */
void raid_shutdown();

#endif
