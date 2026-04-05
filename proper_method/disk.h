#ifndef DISK_H
#define DISK_H

#include <stdio.h>

/* Constants */
#define MAX_DISKS 20

/**
 * Open a disk file (e.g., RAIDX_no.txt) in the specified mode.
 * Simulated disk files are stored as plain text.
 */
int disk_open(const char* raidx, int disk_id, const char* mode);

/**
 * Close a disk file if it is open.
 */
void disk_close(int disk_id);

/**
 * Read a bit (character '0' or '1') from the given position.
 */
int disk_read_bit(int disk_id, int pos);

/**
 * Write a bit (character '0' or '1') to the given position.
 */
int disk_write_bit(int disk_id, int pos, char bit);

/**
 * Initialize a disk file with '0' up to the given size.
 */
int disk_init(int disk_id, int size);

/**
 * Delete a disk file (simulating hardware failure).
 */
void disk_remove(const char* raidx, int disk_id);

#endif
