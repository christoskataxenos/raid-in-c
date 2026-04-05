#include "raid.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 9) {
        printf("Usage: %s RAIDX SIZE BLOCKSIZE N inputFile updates backup allData\n", argv[0]);
        return 1;
    }

    char *raidx = argv[1];
    int size = atoi(argv[2]);
    int blocksize = atoi(argv[3]);
    int n = atoi(argv[4]);
    char *input_file = argv[5];
    char *updates_file = argv[6];
    char *backup_file = argv[7];
    char *alldata_file = argv[8];

    printf("--- RAID Proper Method (%s) ---\n", raidx);

    /* 1. Init Disks */
    printf("[STEP 1] Initializing %d disks...\n", n);
    if (!raid_init(raidx, n, size)) { printf("[!] Init failed.\n"); return 1; }

    /* 2. Load Input File */
    printf("[STEP 2] Loading from %s...\n", input_file);
    FILE *in = fopen(input_file, "r");
    if (!in) { printf("[!] Error opening input file.\n"); return 1; }
    int ch, count = 0;
    while ((ch = fgetc(in)) != EOF) {
        if (ch == '0' || ch == '1') {
            raid_write_bit(raidx, count++, n, blocksize, (char)ch, 0);
        }
    }
    fclose(in);
    printf(">> Loaded %d bits.\n", count);

    /* 3. Create Backup */
    printf("[STEP 3] Creating backup %s...\n", backup_file);
    in = fopen(input_file, "r");
    FILE *bak = fopen(backup_file, "w+");
    while ((ch = fgetc(in)) != EOF) if (ch == '0' || ch == '1') fputc(ch, bak);
    fclose(in); rewind(bak);

    /* 4. Updates */
    printf("[STEP 4] Processing updates from %s...\n", updates_file);
    FILE *upd = fopen(updates_file, "r");
    int i_pos; char i_val;
    if (upd) {
        while (fscanf(upd, "%d %c", &i_pos, &i_val) == 2) {
            if (i_pos < count) {
                /* Update backup for verification later */
                fseek(bak, i_pos, SEEK_SET); fputc(i_val, bak);
                /* RAID update */
                raid_write_bit(raidx, i_pos, n, blocksize, i_val, 1);
            } else printf(" - Bit %d: [!] Out of bounds.\n", i_pos);
        }
        fclose(upd);
    }
    fclose(bak);

    /* 5. Fail Disk */
    int failed;
    printf("\n[STEP 5] Which disk failed (0-%d): ", n - 1);
    scanf("%d", &failed);
    disk_remove(raidx, failed);
    printf(">> Disk %d has been removed.\n", failed);

    /* 6. Recover */
    printf("[STEP 6] Recovering disk %d...\n", failed);
    raid_recover(raidx, failed, n, size);
    printf(">> Disk %d recovered.\n", failed);

    /* 7. Reconstruct */
    printf("[STEP 7] Reconstructing to %s...\n", alldata_file);
    FILE *out = fopen(alldata_file, "w");
    for (int i = 0; i < count; i++) {
        fputc(raid_read_bit(raidx, i, n, blocksize), out);
    }
    fclose(out);

    /* 8. Compare */
    printf("[STEP 8] Verification...\n");
    raid_compare(alldata_file, backup_file);

    raid_shutdown();
    return 0;
}

void raid_shutdown() {
    for (int i = 0; i < MAX_DISKS; i++) disk_close(i);
}
