#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <openssl/evp.h>

#define CRACK_COUNT 10
#define PASSWORD_MAXLENGTH 6
#define SHA3_LENGTH 32
#define DIGEST_LENGTH 65
#define LOWER_ASCII 'a'
#define UPPER_ASCII 'z'

// --- GLOBALS ---
char uncracked_digests[CRACK_COUNT][DIGEST_LENGTH];
char cracked_passwords[CRACK_COUNT][PASSWORD_MAXLENGTH]; // TASK 3: Write your answers here!
int next_uncracked_digest_index = 0;


// ==========================================
// UTILITY FUNCTIONS (DO NOT MODIFY)
// ==========================================
void increment(char *password) {
    int idx = PASSWORD_MAXLENGTH - 2;
    int done = -1;
    char ltr;
    while (done != 0) {
        ltr = password[idx];
        if (ltr >= UPPER_ASCII) {
            int reset_idx = idx;
            while ((reset_idx >= 0) && (password[reset_idx] >= UPPER_ASCII)) {
                password[reset_idx] = LOWER_ASCII;
                if (reset_idx == 0) done = 0;
                else reset_idx--;
            }
            if (reset_idx == -1) done = 0;
            else {
                password[reset_idx] += 1;
                done = 0;
            }
        } else {
            password[idx] += 1;
            done = 0;
        }
    }
}

void compute_sha3_hash(const char *password, char *result_hex) {
    EVP_MD_CTX *context = EVP_MD_CTX_new();
    const EVP_MD *md = EVP_sha3_256();
    unsigned char this_hash[SHA3_LENGTH];

    EVP_DigestInit_ex(context, md, NULL);
    EVP_DigestUpdate(context, password, PASSWORD_MAXLENGTH - 1);
    EVP_DigestFinal_ex(context, this_hash, NULL);

    for(int j = 0; j < SHA3_LENGTH; j++) {
        sprintf(result_hex + (j * 2), "%02x", this_hash[j]);
    }
    result_hex[DIGEST_LENGTH - 1] = '\0';
    EVP_MD_CTX_free(context);
}

// ==========================================
// PART 1 "Naive" Code
// ==========================================
void* naive_worker(void* arg) {
    int thread_num = *(int*)arg;
    char test_password[PASSWORD_MAXLENGTH];
    char hash_string[DIGEST_LENGTH];

    while (next_uncracked_digest_index < CRACK_COUNT) {

        int my_index = next_uncracked_digest_index;
        if (my_index >= CRACK_COUNT) break;

        printf("[Thread %d] Attempting to crack index %d...\n", thread_num, my_index);
        next_uncracked_digest_index = my_index + 1;

        char *target_digest = uncracked_digests[my_index];

        strcpy(test_password, "aaaaa");
        int cracked = 0;

        while (!cracked) {
            compute_sha3_hash(test_password, hash_string);

            if (strncmp(target_digest, hash_string, DIGEST_LENGTH - 1) == 0) {
                printf("[Thread %d] SUCCESS! Index %d is: %s\n", thread_num, my_index, test_password);
                cracked = 1;
            } else {
                increment(test_password);
            }
        }
    }
    return NULL;
}

// ==========================================
// Task 2: The "Smart" Solution
// ==========================================

// TODO: Define your 'struct thread_args' here!
struct thread_args{
    int thread_id, start_index, end_index;
};

// TODO: Write your 'smart_worker' function here!
// Remember: No printf allowed! Write matches to cracked_passwords[i].
void* smart_worker(void* arg){
    struct thread_args* args = (struct thread_args*)arg;
    char test_password[PASSWORD_MAXLENGTH];
    char hash_string[DIGEST_LENGTH];

    for(int i = args->start_index; i < args->end_index; i++){
        char *target_digest = uncracked_digests[i];

        strcpy(test_password, "aaaaa");
        int cracked = 0;

        while (!cracked) {
            compute_sha3_hash(test_password, hash_string);

            if (strncmp(target_digest, hash_string, DIGEST_LENGTH - 1) == 0) {
                strcpy(cracked_passwords[i], test_password);
                cracked = 1;
            } else {
                increment(test_password);
            }
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: ./crack <digest_file> <num_threads>\n");
        exit(1);
    }

    int num_threads = atoi(argv[2]);
    if (num_threads < 1) num_threads = 1;

    FILE *f = fopen(argv[1], "r");
    if (!f) {
        fprintf(stderr, "Failed to open %s.\n", argv[1]);
        exit(1);
    }

    char *line = NULL;
    size_t len = 0;
    int line_count = 0;
    while ((getline(&line, &len, f) != -1) && (line_count < CRACK_COUNT)) {
        strncpy(uncracked_digests[line_count], line, DIGEST_LENGTH - 1);
        line_count++;
    }
    fclose(f);
    free(line);

    pthread_t threads[num_threads];
    int thread_ids[num_threads];

    printf("--- STARTING CRACKER WITH %d THREADS ---\n", num_threads);

    // ==========================================
    // TODO: TASK 1 (The Trap)
    // ==========================================
    // 1. Write a loop to create 'num_threads' threads, passing 'naive_worker'.
    // 2. Write a loop to pthread_join all threads.
    
    /*
    for(int i = 0; i < num_threads;){
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, naive_worker, &thread_ids[i]);
    }
    for(int i = 0; i < num_threads; i++){
        pthread_join(threads[i], NULL);
    }
    */


    // ==========================================
    // TODO: TASK 3 (The Fix)
    // ==========================================
    // 1. Comment out your Task 1 loops above. Do not delete them!
    // 2. Dynamically allocate an array of your custom struct.
    // 3. Write the math to divide CRACK_COUNT evenly across 'num_threads'.
    // 4. Create the threads, passing the specific struct pointer to each.
    // 5. Join the threads.
    // 6. Loop through cracked_passwords and print the final results!
    // 7. free() your dynamically allocated structs.

    struct thread_args* args = malloc(num_threads*sizeof(struct thread_args));
    int evenly_div = CRACK_COUNT / num_threads;
    int remainder = CRACK_COUNT % num_threads;

    int curr = 0;
    for(int i = 0; i < num_threads; i++){
        args[i].thread_id = i;

        int num_to_crack = evenly_div;
        if(i<remainder)
            num_to_crack = evenly_div+1;

        args[i].start_index = curr;
        args[i].end_index = curr+num_to_crack;
        curr += num_to_crack;

        pthread_create(&threads[i], NULL, smart_worker, &args[i]);
    }
    for(int i = 0; i < num_threads; i++){
        pthread_join(threads[i], NULL);
    }

    for(int i = 0; i< CRACK_COUNT; i++){
        printf("%d: %s\n", i, cracked_passwords[i]);
    }

    free(args);

    return 0;
}
