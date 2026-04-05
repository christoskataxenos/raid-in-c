#ifndef RAID_H
#define RAID_H

#include "disk.h"

/* RAID Levels */
#define RAID_1 1
#define RAID_1E 12 /* RAID 1 Enhanced */
#define RAID_4 4
#define RAID_5 5

/* 
 * raid_init
 * Ανοίγει τους N δίσκους και τους αρχικοποιεί με '0'.
 */
int raid_init(const char* raidx, int n, int size);

/* 
 * raid_write_bit
 * Γράφει ένα bit στο RAID σύστημα σύμφωνα με τον αλγόριθμο.
 */
int raid_write_bit(const char* raidx, int pos, int n, int blocksize, char bit, int is_update);

/* 
 * raid_read_bit
 * Διαβάζει ένα bit από το RAID σύστημα.
 */
char raid_read_bit(const char* raidx, int pos, int n, int blocksize);

/* 
 * raid_recover
 * Ανάκτηση του κατεστραμμένου δίσκου.
 */
void raid_recover(const char* raidx, int failed_id, int n, int size);

/* 
 * raid_compare
 * Σύγκριση δύο αρχείων για 100% ομοιότητα.
 */
void raid_compare(const char* file1, const char* file2);

/* 
 * raid_shutdown
 * Κλείνει όλους τους δίσκους.
 */
void raid_shutdown();

#endif

