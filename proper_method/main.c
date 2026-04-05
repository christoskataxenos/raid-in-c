#include "raid.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    char raidx[10], input_file[64], updates_file[64], backup_file[64], alldata_file[64];
    int size, blocksize, n, i, ch, data_bits = 0, failed_id;
    FILE *in, *bak, *upd, *out;
    double cap_multiplier;
    int total_capacity;

    printf("--- RAID SIMULATOR: PROPER METHOD ---\n\n");

    /* 1. Argument Acquisition (Dual-Mode Input) */
    if (argc >= 9) {
        strncpy(raidx, argv[1], 9);
        size = atoi(argv[2]);
        blocksize = atoi(argv[3]);
        n = atoi(argv[4]);
        strncpy(input_file, argv[5], 63);
        strncpy(updates_file, argv[6], 63);
        strncpy(backup_file, argv[7], 63);
        strncpy(alldata_file, argv[8], 63);
    } else {
        printf("Interactive Setup Mode:\n");
        printf("Enter RAID level (RAID1, RAID1E, RAID4, RAID5): "); scanf("%s", raidx);
        printf("Enter Disk Size (bits): "); scanf("%d", &size);
        printf("Enter Block Size (bits): "); scanf("%d", &blocksize);
        printf("Enter Number of Disks (N): "); scanf("%d", &n);
        printf("Enter Input Filename: "); scanf("%s", input_file);
        printf("Enter Updates Filename: "); scanf("%s", updates_file);
        printf("Enter Backup Filename: "); scanf("%s", backup_file);
        printf("Enter Reconstructed Filename: "); scanf("%s", alldata_file);
    }

    /* Validating Multiplier */
    if (strcmp(raidx, "RAID1") == 0) cap_multiplier = 1.0;
    else if (strcmp(raidx, "RAID1E") == 0) cap_multiplier = n / 2.0;
    else cap_multiplier = n - 1.0;

    /* STEP 1: Initialization */
    printf("\n[STEP 1] Initializing %d disks...\n", n);
    if (!raid_init(raidx, n, size)) { printf("[ERROR] RAID initialization failed.\n"); return 1; }

    /* STEP 2: Loading Data */
    printf("[STEP 2] Loading data from source: %s...\n", input_file);
    in = fopen(input_file, "r");
    if (!in) { printf("[ERROR] Input file missing.\n"); return 1; }
    
    total_capacity = (int)(size * cap_multiplier);
    while ((ch = fgetc(in)) != EOF) {
        if (ch != '0' && ch != '1') continue;
        if (data_bits >= total_capacity) break;
        raid_write_bit(raidx, data_bits, n, blocksize, (char)ch, 0);
        data_bits++;
    }
    fclose(in);
    printf(">> Loaded %d bits of data.\n", data_bits);

    /* STEP 3: Reference Persistence */
    printf("[STEP 3] Creating backup file %s...\n", backup_file);
    in = fopen(input_file, "r");
    bak = fopen(backup_file, "w+");
    i = 0;
    while ((ch = fgetc(in)) != EOF && i < data_bits) {
        if (ch == '0' || ch == '1') { fputc(ch, bak); i++; }
    }
    fclose(in); rewind(bak);

    /* STEP 4: Updates */
    printf("[STEP 4] Processing bitwise updates from %s...\n", updates_file);
    upd = fopen(updates_file, "r");
    if (upd) {
        int u_pos; char u_val;
        while (fscanf(upd, "%d %c", &u_pos, &u_val) == 2) {
            if (u_pos < data_bits) {
                fseek(bak, u_pos, SEEK_SET); fputc(u_val, bak);
                raid_write_bit(raidx, u_pos, n, blocksize, u_val, 1);
            }
        }
        fclose(upd);
    }
    fclose(bak);

    /* STEP 5: Critical Failure Simulation */
    printf("\n[STEP 5] Select disk id to destroy (0-%d): ", n - 1);
    if (scanf("%d", &failed_id) != 1) failed_id = 0;
    disk_remove(raidx, failed_id);
    printf(">> Disk %d has been removed (Simulated Failure).\n", failed_id);

    /* STEP 6: System Recovery */
    printf("[STEP 6] Recovering disk %d using bitwise parity/mirrors...\n", failed_id);
    raid_recover(raidx, failed_id, n, size);
    printf(">> Recovery protocol completed.\n");

    /* STEP 7: Reassembly */
    printf("[STEP 7] Reconstructing all logical data to %s...\n", alldata_file);
    out = fopen(alldata_file, "w");
    if (!out) { printf("[ERROR] Output file creation failed.\n"); return 1; }
    for (i = 0; i < data_bits; i++) {
        fputc(raid_read_bit(raidx, i, n, blocksize), out);
    }
    fclose(out);

    /* STEP 8: Verification Audit */
    printf("[STEP 8] Performing final data integrity audit...\n");
    raid_compare(alldata_file, backup_file);

    raid_shutdown();
    printf("\nProcess finished successfully.\n");
    return 0;
}
