#ifndef ENV_H
#define ENV_H

#include "disk.h"

/* Ο φάκελος backup που δημιούργησε ο χρήστης */
#define BACKUP_DIR "arxikoi_diskoi"

/* 
 * env_reset_to_backups
 * Μετακινεί τα υπάρχοντα αρχεία diskX.bin στον φάκελο BACKUP_DIR.
 * Χρήσιμο για να ξεκινάμε από "λευκή σελίδα".
 */
int env_reset_to_backups(int disk_count);

/* 
 * env_prepare_disks
 * Σιγουρεύεται ότι τα αρχεία των δίσκων υπάρχουν.
 * Αν δεν υπάρχουν, τα δημιουργεί (κενά).
 */
int env_prepare_disks(int disk_count);

#endif
