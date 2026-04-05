#include "env.h"
#include <stdio.h>
#include <string.h>

/*
 * Στα Windows / ANSI C χρησιμοποιούμε τη rename() για μετακίνηση.
 * Αν το αρχείο-προορισμός υπάρχει, η rename() μπορεί να αποτύχει σε ορισμένες υλοποιήσεις.
 * Εδώ υποθέτουμε ότι ο φάκελος arxikoi_diskoi είναι έτοιμος.
 */
int env_reset_to_backups(int disk_count) {
    int i;
    char src[64];
    char dest[128];
    
    for (i = 0; i < disk_count; i++) {
        sprintf(src, "disk%d.bin", i);
        /* Σημείωση: Το path πρέπει να είναι σωστό για το περιβάλλον (Windows \) */
        sprintf(dest, "%s/disk%d.bin", BACKUP_DIR, i);
        
        /* 
         * Προσπαθούμε να μετακινήσουμε το αρχείο. 
         * Αν ο δίσκος δεν υπάρχει ακόμα, η rename() θα αποτύχει, 
         * κάτι που είναι αποδεκτό (ignore).
         */
        if (rename(src, dest) == 0) {
            printf("[ENV] Move: %s -> %s\n", src, dest);
        }
    }
    
    /* Αφού μετακινήσαμε τους παλιούς, δημιουργούμε νέους άδειους δίσκους */
    return env_prepare_disks(disk_count);
}

/*
 * env_prepare_disks
 * Απλό άνοιγμα σε binary write (wb) για να δημιουργήσει το αρχείο αν δεν υπάρχει.
 * Προσοχή: Το "wb" καθαρίζει το αρχείο αν υπάρχει!
 */
int env_prepare_disks(int disk_count) {
    int i;
    for (i = 0; i < disk_count; i++) {
        /* Αν χρησιμοποιήσουμε "ab+", το αρχείο δημιουργείται αν δεν υπάρχει χωρίς να σβήνεται. */
        if (disk_open(i, "ab+")) {
            disk_close(i);
        } else {
            printf("[ERROR] Failed to prepare disk%d.bin\n", i);
            return 0;
        }
    }
    return 1;
}
