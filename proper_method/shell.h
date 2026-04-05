#ifndef SHELL_H
#define SHELL_H

/* 
 * shell_loop
 * Ο κύριος βρόχος του προγράμματος. 
 * Περιμένει εντολές από τον χρήστη και τις εκτελεί.
 */
void shell_loop();

/* 
 * shell_startup_menu
 * Ρωτάει τον χρήστη για το RAID level και τον αριθμό δίσκων στην αρχή.
 */
void shell_startup_menu();

/* 
 * shell_print_help
 * Εμφανίζει τις διαθέσιμες εντολές.
 */
void shell_print_help();

#endif
