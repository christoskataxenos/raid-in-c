#ifndef RAID_H
#define RAID_H

#include "disk.h"

/* RAID Levels */
#define RAID_0 0
#define RAID_1 1
#define RAID_5 5

/* 
 * raid_init
 * Αρχικοποίηση της συστοιχίας RAID.
 */
int raid_init(int level, int disk_count);

/* 
 * raid_write_block
 * Γράφει δεδομένα μεγέθους 512 bytes σε ένα logical block.
 */
int raid_write_block(int logical_block_id, const byte* data);

/* 
 * raid_read_block
 * Διαβάζει δεδομένα 512 bytes από ένα logical block.
 */
int raid_read_block(int logical_block_id, byte* buffer);

/* 
 * raid_fail_disk
 * Προσομοίωση αποτυχίας ενός φυσικού δίσκου.
 */
void raid_fail_disk(int disk_id);

/* 
 * raid_recover_disk
 * Προσπάθεια ανάκτησης δεδομένων για έναν δίσκο.
 */
int raid_recover_disk(int disk_id);

/* 
 * raid_shutdown
 * Κλείσιμο όλων των δίσκων.
 */
void raid_shutdown();

/* 
 * raid_print_status
 * Εμφάνιση κατάστασης (Online/Failed) για κάθε δίσκο.
 */
void raid_print_status();

#endif
