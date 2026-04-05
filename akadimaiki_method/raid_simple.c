#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* --- Σταθερές --- */
#define MAX_DISKS 20

/* 
 * raid_simple.c (Ακαδημαϊκή Μέθοδος - Single File)
 * Υλοποίηση σύμφωνα με την επίσημη εκφώνηση της άσκησης.
 * Σειρά 1-8: Δημιουργία -> Φόρτωμα -> Backup -> Updates -> Fail -> Recovery -> Reconstruct -> Compare.
 */

/* Βοηθητική συνάρτηση για σύγκριση αρχείων (Step 8) */
void compare_files(const char *file1, const char *file2) {
    FILE *f1 = fopen(file1, "r");
    FILE *f2 = fopen(file2, "r");
    int ch1, ch2;
    int match = 1;

    if (!f1 || !f2) {
        printf("[ΣΦΑΛΜΑ] Αδυναμία ανοίγματος αρχείων για σύγκριση.\n");
        if (f1) fclose(f1); if (f2) fclose(f2);
        return;
    }

    while (1) {
        ch1 = fgetc(f1);
        ch2 = fgetc(f2);
        if (ch1 != ch2) { match = 0; break; }
        if (ch1 == EOF) break;
    }

    if (match) printf(">> 100%% similarity\n");
    else printf(">> Files do not match! Similarity is not 100%%\n");

    fclose(f1); fclose(f2);
}

int main(int argc, char *argv[]) {
    /* 
     * Ορίσματα: 
     * 1: RAIDX, 2: SIZE, 3: BLOCKSIZE, 4: N, 
     * 5: inputFile, 6: updatesFile, 7: backupFile, 8: allDataFile 
     */
    if (argc < 9) {
        printf("Χρήση: %s RAIDX SIZE BLOCKSIZE N inputFile updates backup allData\n", argv[0]);
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

    FILE *disks[MAX_DISKS];
    char filename[64];
    int i, j;

    printf("--- ΠΡΟΣΟΜΟΙΩΤΗΣ RAID (%s, N=%d, BLOCK=%d, SIZE=%d) ---\n", raidx, n, blocksize, size);

    /* 1. Δημιουργία N αρχείων κειμένου (δίσκοι) και αρχικοποίηση στο '0' */
    printf("[STEP 1] Αρχικοποίηση %d δίσκων...\n", n);
    for (i = 0; i < n; i++) {
        sprintf(filename, "%s_%d.txt", raidx, i);
        disks[i] = fopen(filename, "w+");
        if (!disks[i]) { printf("Σφάλμα στον δίσκο %d\n", i); return 1; }
        for (j = 0; j < size; j++) fputc('0', disks[i]);
        rewind(disks[i]);
    }

    /* 2. Φόρτωμα δεδομένων από το inputFile.txt στο RAID σύστημα */
    printf("[STEP 2] Φόρτωμα δεδομένων από %s...\n", input_file);
    FILE *in = fopen(input_file, "r");
    if (!in) { printf("[ΣΦΑΛΜΑ] Αδυναμία ανοίγματος του αρχείου εισόδου '%s'.\n", input_file); return 1; }

    /* Capacity calculation:
     * RAID 1: 1 disk worth of data.
     * RAID 1E: Total space / 2 (each bit written twice).
     * RAID 4/5: (N-1) disks worth of data.
     */
    double cap_multiplier = (strcmp(raidx, "RAID1") == 0) ? 1.0 : 
                            (strcmp(raidx, "RAID1E") == 0) ? n / 2.0 : (n - 1.0);
    int total_capacity = (int)(size * cap_multiplier);
    int actual_data_bits = 0;
    int ch;

    while ((ch = fgetc(in)) != EOF) {
        if (ch != '0' && ch != '1') continue;
        int bit = ch - '0';
        actual_data_bits++;

        if (actual_data_bits > total_capacity) {
            printf("[ΣΦΑΛΜΑ] Η πληροφορία (%d bits) υπερβαίνει τη χωρητικότητα (%d bits)!\n", actual_data_bits, total_capacity);
            fclose(in); return 1;
        }

        /* Λογική RAID */
        if (strcmp(raidx, "RAID1") == 0) {
            for (i = 0; i < n; i++) {
                fseek(disks[i], actual_data_bits - 1, SEEK_SET);
                fputc(ch, disks[i]);
            }
        } else if (strcmp(raidx, "RAID1E") == 0) {
            /* RAID 1E Adjacent Mirroring */
            int d = (actual_data_bits - 1) % n;
            fseek(disks[d], (actual_data_bits - 1), SEEK_SET); fputc(ch, disks[d]);
            int m = (d + 1) % n;
            fseek(disks[m], (actual_data_bits - 1), SEEK_SET); fputc(ch, disks[m]);
        } else if (strcmp(raidx, "RAID4") == 0) {
            int target_disk = ( (actual_data_bits-1) / blocksize ) % (n-1);
            int local_pos = ( (actual_data_bits-1) / (blocksize * (n-1)) ) * blocksize + ( (actual_data_bits-1) % blocksize );
            fseek(disks[target_disk], local_pos, SEEK_SET); fputc(ch, disks[target_disk]);
            
            /* Update Parity */
            fseek(disks[n-1], local_pos, SEEK_SET);
            int old_parity = fgetc(disks[n-1]) - '0';
            fseek(disks[n-1], local_pos, SEEK_SET);
            fputc((old_parity ^ bit) + '0', disks[n-1]);
        } else if (strcmp(raidx, "RAID5") == 0) {
            int stripe = (actual_data_bits - 1) / (blocksize * (n-1));
            int block_in_stripe = ((actual_data_bits - 1) % (blocksize * (n-1))) / blocksize;
            int parity_disk = stripe % n;
            int target_disk = (block_in_stripe >= parity_disk) ? block_in_stripe + 1 : block_in_stripe;
            int local_pos = stripe * blocksize + ((actual_data_bits - 1) % blocksize);
            
            fseek(disks[target_disk], local_pos, SEEK_SET); fputc(ch, disks[target_disk]);
            fseek(disks[parity_disk], local_pos, SEEK_SET);
            int old_p = fgetc(disks[parity_disk]) - '0';
            fseek(disks[parity_disk], local_pos, SEEK_SET);
            fputc((old_p ^ bit) + '0', disks[parity_disk]);
        }
    }
    fclose(in);

    /* 3. Δημιουργία backup.txt */
    printf("[STEP 3] Δημιουργία backup αρχείου %s...\n", backup_file);
    in = fopen(input_file, "r");
    FILE *bak = fopen(backup_file, "w+");
    while ((ch = fgetc(in)) != EOF) if (ch == '0' || ch == '1') fputc(ch, bak);
    fclose(in); rewind(bak);

    /* 4. Επεξεργασία updates.txt */
    printf("[STEP 4] Επεξεργασία αλλαγών από %s...\n", updates_file);
    FILE *upd = fopen(updates_file, "r");
    int pos; char newVal;
    if (!upd) {
        printf("[!] Προειδοποίηση: Το αρχείο updates '%s' δεν βρέθηκε. Συνέχεια χωρίς αλλαγές.\n", updates_file);
    } else {
        while (fscanf(upd, "%d %c", &pos, &newVal) == 2) {
            if (pos >= actual_data_bits) {
                printf(" - Bit %d: ΛΑΘΟΣ (Εκτός ορίων)\n", pos); continue;
            }
            /* Get old value from backup */
            fseek(bak, pos, SEEK_SET); char oldVal = fgetc(bak);
            fseek(bak, pos, SEEK_SET); fputc(newVal, bak);

            if (oldVal != newVal) {
                printf(" - Αλλαγή bit %d από %c σε %c: ", pos, oldVal, newVal);
                int bit_diff = (oldVal - '0') ^ (newVal - '0');

                if (strcmp(raidx, "RAID1") == 0) {
                    for (i = 0; i < n; i++) { fseek(disks[i], pos, SEEK_SET); fputc(newVal, disks[i]); }
                    printf("OK (Mirror)\n");
                } else if (strcmp(raidx, "RAID1E") == 0) {
                    int d = pos % n; fseek(disks[d], pos, SEEK_SET); fputc(newVal, disks[d]);
                    int m = (d + 1) % n; fseek(disks[m], pos, SEEK_SET); fputc(newVal, disks[m]);
                    printf("OK (MirrorE)\n");
                } else if (strcmp(raidx, "RAID4") == 0) {
                    int target = (pos / blocksize) % (n-1);
                    int local = (pos / (blocksize * (n-1))) * blocksize + (pos % blocksize);
                    fseek(disks[target], local, SEEK_SET); fputc(newVal, disks[target]);
                    fseek(disks[n-1], local, SEEK_SET); int p = fgetc(disks[n-1]) - '0';
                    fseek(disks[n-1], local, SEEK_SET); fputc((p ^ bit_diff) + '0', disks[n-1]);
                    printf("Update Parity (Disk %d) -> XOR\n", n-1);
                } else if (strcmp(raidx, "RAID5") == 0) {
                    int stripe = pos / (blocksize * (n-1));
                    int b_in_s = (pos % (blocksize * (n-1))) / blocksize;
                    int p_disk = stripe % n;
                    int target = (b_in_s >= p_disk) ? b_in_s + 1 : b_in_s;
                    int local = stripe * blocksize + (pos % blocksize);
                    fseek(disks[target], local, SEEK_SET); fputc(newVal, disks[target]);
                    fseek(disks[p_disk], local, SEEK_SET); int p = fgetc(disks[p_disk]) - '0';
                    fseek(disks[p_disk], local, SEEK_SET); fputc((p ^ bit_diff) + '0', disks[p_disk]);
                    printf("Update Parity (Disk %d) -> XOR\n", p_disk);
                }
            } else printf(" - Bit %d: Ήδη %c (Καμία αλλαγή)\n", pos, newVal);
        }
        fclose(upd);
    }
    if (bak) fclose(bak);

    /* 5. Προσομοίωση Σφάλματος */
    int failed;
    printf("\n[STEP 5] Ποιος δίσκος θα παρουσιάσει σφάλμα (0-%d): ", n - 1);
    scanf("%d", &failed);
    fclose(disks[failed]);
    sprintf(filename, "%s_%d.txt", raidx, failed);
    remove(filename);
    printf(">> Ο δίσκος %s διαγράφηκε!\n", filename);

    /* 6. Ανάκτηση Δεδομένων */
    printf("[STEP 6] Ανάκτηση δεδομένων δίσκου %d...\n", failed);
    disks[failed] = fopen(filename, "w+");
    for (i = 0; i < size; i++) {
        if (strcmp(raidx, "RAID1") == 0) {
            int src = (failed == 0) ? 1 : 0;
            fseek(disks[src], i, SEEK_SET); fputc(fgetc(disks[src]), disks[failed]);
        } else if (strcmp(raidx, "RAID1E") == 0) {
            /* Basic adjacent mirror recovery */
            int src = (failed + 1) % n;
            fseek(disks[src], i, SEEK_SET); 
            int val = fgetc(disks[src]);
            if (val == EOF) { /* Try the other side if needed */
                src = (failed + n - 1) % n;
                fseek(disks[src], i, SEEK_SET);
                val = fgetc(disks[src]);
            }
            fputc(val, disks[failed]);
        } else { /* XOR Recovery for RAID4/5 */
            int recovered = 0;
            for (j = 0; j < n; j++) {
                if (j == failed) continue;
                fseek(disks[j], i, SEEK_SET);
                recovered ^= (fgetc(disks[j]) - '0');
            }
            fputc(recovered + '0', disks[failed]);
        }
    }
    printf(">> Ανάκτηση ολοκληρώθηκε.\n");

    /* 7. Επαναδομή στο allData.txt */
    printf("[STEP 7] Επαναδημιουργία πληροφορίας στο %s...\n", alldata_file);
    FILE *out = fopen(alldata_file, "w");
    for (i = 0; i < actual_data_bits; i++) {
        char bit;
        if (strcmp(raidx, "RAID1") == 0) { fseek(disks[0], i, SEEK_SET); bit = fgetc(disks[0]); }
        else if (strcmp(raidx, "RAID1E") == 0) { fseek(disks[i % n], i, SEEK_SET); bit = fgetc(disks[i % n]); }
        else if (strcmp(raidx, "RAID4") == 0) {
            int target = (i / blocksize) % (n-1);
            int local = (i / (blocksize * (n-1))) * blocksize + (i % blocksize);
            fseek(disks[target], local, SEEK_SET); bit = fgetc(disks[target]);
        } else { /* RAID5 */
            int stripe = i / (blocksize * (n-1));
            int b_in_s = (i % (blocksize * (n-1))) / blocksize;
            int p_disk = stripe % n;
            int target = (b_in_s >= p_disk) ? b_in_s + 1 : b_in_s;
            int local = stripe * blocksize + (i % blocksize);
            fseek(disks[target], local, SEEK_SET); bit = fgetc(disks[target]);
        }
        fputc(bit, out);
    }
    fclose(out);

    /* 8. Σύγκριση */
    printf("[STEP 8] Τελικός έλεγχος σύγκρισης...\n");
    compare_files(alldata_file, backup_file);

    /* Cleanup */
    for (i = 0; i < n; i++) fclose(disks[i]);
    return 0;
}
