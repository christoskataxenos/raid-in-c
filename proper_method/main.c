#include "shell.h"
#include <stdio.h>

/*
 * RAID Simulator Project
 * ----------------------
 * Συμβατό με ANSI C / C89 για Windows 2000.
 * Η εκτέλεση ξεκινά από το Interactive Shell.
 */
int main() {
    printf("========================================\n");
    printf("      RAID STORAGE SIMULATOR (v1.0)     \n");
    printf("========================================\n");
    
    /* Αρχικό μενού ρύθμισης */
    shell_startup_menu();
    
    /* Έναρξη του interactive περιβάλλοντος */
    shell_loop();
    
    printf("\nΤερματισμός Εφαρμογής. Αντίο!\n");
    return 0;
}
