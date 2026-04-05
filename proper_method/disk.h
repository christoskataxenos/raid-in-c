#ifndef DISK_H
#define DISK_H

#include <stdio.h>

/* Μεταβλητές & Σταθερές */
#define BLOCK_SIZE 512
#define MAX_DISKS 10

typedef unsigned char byte;

/* 
 * Ανοίγει ένα αρχείο δίσκου (π.χ. disk0.bin) 
 * Επιστρέφει 1 για επιτυχία, 0 για αποτυχία.
 */
int disk_open(int disk_id, const char* mode);

/* 
 * Κλείνει ένα αρχείο δίσκου αν είναι ανοιχτό.
 */
void disk_close(int disk_id);

/* 
 * Διαβάζει ένα block 512 bytes από τη θέση block_id στον buffer.
 * Επιστρέφει 1 για επιτυχία, 0 για αποτυχία.
 */
int disk_read_block(int disk_id, int block_id, byte* buffer);

/* 
 * Γράφει ένα block 512 bytes από τον buffer στη θέση block_id.
 * Επιστρέφει 1 για επιτυχία, 0 για αποτυχία.
 */
int disk_write_block(int disk_id, int block_id, const byte* buffer);

/* 
 * Ελέγχει αν ο δίσκος είναι ανοιχτός/διαθέσιμος.
 */
int disk_is_ready(int disk_id);

#endif
