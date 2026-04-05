#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * RAID Simulator (Academic Method)
 * Implements RAID 1, 1E, 4, 5 using modular functions and function pointers.
 * All messages and comments are in English as requested.
 */

#define MAX_DISKS 20

/* --- Function Pointer Type Definitions --- */
typedef void (*WriteBitFunc)(FILE **, int, char, int, int, int *, int *, int *);
typedef void (*RecoverFunc)(FILE **, int, int, int);
typedef void (*ReconstructFunc)(FILE **, FILE *, int, int, int, int);

/* --- RAID 1: Mirroring --- */
void raid1_write(FILE **disks, int n, char bit, int total_bits, int blocksize, int *curr_disk, int *bits_in_blk, int *parity) {
    int i;
    for (i = 0; i < n; i++) {
        fseek(disks[i], total_bits, SEEK_SET);
        fputc(bit, disks[i]);
    }
}

void raid1_recover(FILE **disks, int n, int failed, int size) {
    int i, source = (failed == 0) ? 1 : 0;
    rewind(disks[source]);
    for (i = 0; i < size; i++) {
        fputc(fgetc(disks[source]), disks[failed]);
    }
}

void raid1_reconstruct(FILE **disks, FILE *out, int n, int data_bits, int blocksize, int data_disks) {
    int i;
    rewind(disks[0]);
    for (i = 0; i < data_bits; i++) {
        fputc(fgetc(disks[0]), out);
    }
}

/* --- RAID 1E: Adjacent Mirroring --- */
void raid1e_write(FILE **disks, int n, char bit, int total_bits, int blocksize, int *curr_disk, int *bits_in_blk, int *parity) {
    int d = total_bits % n;
    int m = (d + 1) % n;
    fseek(disks[d], total_bits, SEEK_SET); fputc(bit, disks[d]);
    fseek(disks[m], total_bits, SEEK_SET); fputc(bit, disks[m]);
}

void raid1e_recover(FILE **disks, int n, int failed, int size) {
    int i;
    for (i = 0; i < size; i++) {
        int src = (failed + 1) % n;
        fseek(disks[src], i, SEEK_SET);
        fputc(fgetc(disks[src]), disks[failed]);
    }
}

void raid1e_reconstruct(FILE **disks, FILE *out, int n, int data_bits, int blocksize, int data_disks) {
    int i;
    for (i = 0; i < data_bits; i++) {
        int d = i % n;
        fseek(disks[d], i, SEEK_SET);
        fputc(fgetc(disks[d]), out);
    }
}

/* --- RAID 4: Dedicated Parity --- */
void raid4_write(FILE **disks, int n, char bit, int total_bits, int blocksize, int *curr_disk, int *bits_in_blk, int *parity) {
    int data_disks = n - 1;
    int target = (total_bits / blocksize) % data_disks;
    int local = (total_bits / (blocksize * data_disks)) * blocksize + (total_bits % blocksize);
    int bit_val = bit - '0';

    fseek(disks[target], local, SEEK_SET); fputc(bit, disks[target]);
    fseek(disks[n-1], local, SEEK_SET);
    *parity = fgetc(disks[n-1]) - '0';
    fseek(disks[n-1], local, SEEK_SET);
    fputc((*parity ^ bit_val) + '0', disks[n-1]);
}

void raid4_reconstruct(FILE **disks, FILE *out, int n, int data_bits, int blocksize, int data_disks) {
    int i;
    for (i = 0; i < data_bits; i++) {
        int target = (i / blocksize) % data_disks;
        int local = (i / (blocksize * data_disks)) * blocksize + (i % blocksize);
        fseek(disks[target], local, SEEK_SET);
        fputc(fgetc(disks[target]), out);
    }
}

/* --- RAID 5: Distributed Parity --- */
void raid5_write(FILE **disks, int n, char bit, int total_bits, int blocksize, int *curr_disk, int *bits_in_blk, int *parity) {
    int data_disks = n - 1;
    int stripe = total_bits / (blocksize * data_disks);
    int block_in_stripe = (total_bits % (blocksize * data_disks)) / blocksize;
    int parity_disk = stripe % n;
    int target = (block_in_stripe >= parity_disk) ? block_in_stripe + 1 : block_in_stripe;
    int local = stripe * blocksize + (total_bits % blocksize);
    int bit_val = bit - '0';

    fseek(disks[target], local, SEEK_SET); fputc(bit, disks[target]);
    fseek(disks[parity_disk], local, SEEK_SET);
    *parity = fgetc(disks[parity_disk]) - '0';
    fseek(disks[parity_disk], local, SEEK_SET);
    fputc((*parity ^ bit_val) + '0', disks[parity_disk]);
}

void raid5_reconstruct(FILE **disks, FILE *out, int n, int data_bits, int blocksize, int data_disks) {
    int i;
    for (i = 0; i < data_bits; i++) {
        int stripe = i / (blocksize * (n - 1));
        int block_in_stripe = (i % (blocksize * (n - 1))) / blocksize;
        int p_disk = stripe % n;
        int target = (block_in_stripe >= p_disk) ? block_in_stripe + 1 : block_in_stripe;
        int local = stripe * blocksize + (i % blocksize);
        fseek(disks[target], local, SEEK_SET);
        fputc(fgetc(disks[target]), out);
    }
}

/* --- XOR Recovery for RAID 4 & 5 --- */
void raid_xor_recover(FILE **disks, int n, int failed, int size) {
    int i, j;
    for (i = 0; i < size; i++) {
        int recovered = 0;
        for (j = 0; j < n; j++) {
            if (j == failed) continue;
            fseek(disks[j], i, SEEK_SET);
            recovered ^= (fgetc(disks[j]) - '0');
        }
        fseek(disks[failed], i, SEEK_SET);
        fputc(recovered + '0', disks[failed]);
    }
}

/* --- Utility Functions --- */
void compare_files(const char *file1, const char *file2) {
    FILE *f1 = fopen(file1, "r");
    FILE *f2 = fopen(file2, "r");
    int ch1, ch2, match = 1;

    if (!f1 || !f2) {
        printf("[ERROR] Cannot open files for comparison.\n");
        if (f1) fclose(f1); if (f2) fclose(f2);
        return;
    }
    while (1) {
        ch1 = fgetc(f1); ch2 = fgetc(f2);
        if (ch1 != ch2) { match = 0; break; }
        if (ch1 == EOF) break;
    }
    if (match) printf(">> 100%% similarity confirmed.\n");
    else printf(">> Files mismatch! Recovery failed.\n");
    fclose(f1); fclose(f2);
}

/* --- Main Logic --- */
int main(int argc, char *argv[]) {
    /* Variables at top (ANSI C) */
    char raidx[10], input_file[64], updates_file[64], backup_file[64], alldata_file[64], filename[64];
    int size, blocksize, n, i, j, ch, actual_data_bits = 0, failed_disk;
    int curr_disk = 0, bits_in_blk = 0, parity = 0;
    FILE *disks[MAX_DISKS], *in, *bak, *upd, *out;
    double cap_multiplier;
    int total_capacity;

    WriteBitFunc write_ptr = NULL;
    RecoverFunc recover_ptr = NULL;
    ReconstructFunc reconstruct_ptr = NULL;

    printf("--- RAID SIMULATOR ---\n");

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
        printf("Interactive Mode:\n");
        printf("Enter RAID Level (RAID1, RAID1E, RAID4, RAID5): "); scanf("%6s", raidx);
        printf("Enter Disk Size (bits): "); scanf("%d", &size);
        printf("Enter Block Size (bits): "); scanf("%d", &blocksize);
        printf("Enter Number of Disks (N): "); scanf("%d", &n);
        printf("Enter Input Filename: "); scanf("%63s", input_file);
        printf("Enter Updates Filename: "); scanf("%63s", updates_file);
        printf("Enter Backup Filename: "); scanf("%63s", backup_file);
        printf("Enter Reconstructed Filename: "); scanf("%63s", alldata_file);
    }

    if (strcmp(raidx, "RAID1") == 0) {
        write_ptr = raid1_write; recover_ptr = raid1_recover; reconstruct_ptr = raid1_reconstruct;
        cap_multiplier = 1.0;
    } else if (strcmp(raidx, "RAID1E") == 0) {
        write_ptr = raid1e_write; recover_ptr = raid1e_recover; reconstruct_ptr = raid1e_reconstruct;
        cap_multiplier = n / 2.0;
    } else if (strcmp(raidx, "RAID4") == 0) {
        write_ptr = raid4_write; recover_ptr = raid_xor_recover; reconstruct_ptr = raid4_reconstruct;
        cap_multiplier = n - 1.0;
    } else if (strcmp(raidx, "RAID5") == 0) {
        write_ptr = raid5_write; recover_ptr = raid_xor_recover; reconstruct_ptr = raid5_reconstruct;
        cap_multiplier = n - 1.0;
    } else {
        printf("[ERROR] Unsupported RAID level.\n"); return 1;
    }

    /* STEP 1: Initialization */
    printf("\n[STEP 1] Initializing %d disks...\n", n);
    for (i = 0; i < n; i++) {
        sprintf(filename, "%s_%d.txt", raidx, i);
        disks[i] = fopen(filename, "w+");
        for (j = 0; j < size; j++) fputc('0', disks[i]);
        rewind(disks[i]);
    }

    /* STEP 2: Loading Data */
    printf("[STEP 2] Loading data from %s...\n", input_file);
    in = fopen(input_file, "r");
    if (!in) { printf("[ERROR] Input file missing.\n"); return 1; }
    
    total_capacity = (int)(size * cap_multiplier);
    while ((ch = fgetc(in)) != EOF) {
        if (ch != '0' && ch != '1') continue;
        if (actual_data_bits >= total_capacity) break;
        write_ptr(disks, n, (char)ch, actual_data_bits, blocksize, &curr_disk, &bits_in_blk, &parity);
        actual_data_bits++;
    }
    fclose(in);

    /* STEP 3: Backup */
    printf("[STEP 3] Creating backup file %s...\n", backup_file);
    bak = fopen(backup_file, "w+");
    in = fopen(input_file, "r");
    i = 0;
    while ((ch = fgetc(in)) != EOF && i < actual_data_bits) {
        if (ch == '0' || ch == '1') { fputc(ch, bak); i++; }
    }
    fclose(in); rewind(bak);

    /* STEP 4: Updates */
    printf("[STEP 4] Processing updates from %s...\n", updates_file);
    upd = fopen(updates_file, "r");
    if (upd) {
        int pos; char newVal;
        while (fscanf(upd, "%d %c", &pos, &newVal) == 2) {
            if (pos < actual_data_bits) {
                fseek(bak, pos, SEEK_SET); fputc(newVal, bak);
                write_ptr(disks, n, newVal, pos, blocksize, &curr_disk, &bits_in_blk, &parity);
            }
        }
        fclose(upd);
    }
    fclose(bak);

    /* STEP 5: Fail */
    printf("\n[STEP 5] Select disk to fail (0-%d): ", n - 1);
    scanf("%d", &failed_disk);
    fclose(disks[failed_disk]);
    sprintf(filename, "%s_%d.txt", raidx, failed_disk);
    remove(filename);
    printf(">> Disk %s destroyed.\n", filename);

    /* STEP 6: Recover */
    printf("[STEP 6] Recovering disk %d...\n", failed_disk);
    disks[failed_disk] = fopen(filename, "w+");
    recover_ptr(disks, n, failed_disk, size);
    printf(">> Recovery completed.\n");

    /* STEP 7: Reconstruct */
    printf("[STEP 7] Reconstructing all data to %s...\n", alldata_file);
    out = fopen(alldata_file, "w");
    for (i = 0; i < n; i++) rewind(disks[i]);
    reconstruct_ptr(disks, out, n, actual_data_bits, blocksize, n - 1);
    fclose(out);

    /* STEP 8: Compare */
    printf("[STEP 8] Final Verification...\n");
    compare_files(alldata_file, backup_file);

    /* Cleanup */
    for (i = 0; i < n; i++) if (disks[i]) fclose(disks[i]);
    return 0;
}
