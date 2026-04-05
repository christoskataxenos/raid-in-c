#ifndef DISK_H
#define DISK_H

#include <stdio.h>

/* Μεταβλητές & Σταθερές */
#define MAX_DISKS 20

/* 
 * Ανοίγει ένα αρχείο δίσκου (π.χ. RAIDX_no.txt) 
 * Χρησιμοποιείται για την προσομοίωση του δίσκου ως αρχείο κειμένου.
 */
int disk_open(const char* raidx, int disk_id, const char* mode);

/* 
 * Κλείνει ένα αρχείο δίσκου αν είναι ανοιχτό.
 */
void disk_close(int disk_id);

/* 
 * Διαβάζει ένα bit (χαρακτήρα '0' ή '1') από τη θέση pos.
 */
int disk_read_bit(int disk_id, int pos);

/* 
 * Γράφει ένα bit (χαρακτήρα '0' ή '1') στη θέση pos.
 */
int disk_write_bit(int disk_id, int pos, char bit);

/* 
 * Αρχικοποιεί τον δίσκο με '0' μέχρι το μέγεθος size.
 */
int disk_init(int disk_id, int size);

/* 
 * Διαγράφει το αρχείο του δίσκου (Προσομοίωση σφάλματος).
 */
void disk_remove(const char* raidx, int disk_id);

#endif
