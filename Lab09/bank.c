#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define NUM_ACCOUNTS 100
#define NUM_THREADS 8
#define TRANSACTIONS_PER_THREAD 100000
#define INITIAL_BALANCE 1000

// Account Structure
typedef struct {
    int id;
    double balance;
    pthread_mutex_t lock;
} account_t;

// Global State
account_t bank[NUM_ACCOUNTS];


void transfer(int from_idx, int to_idx, double amount) {
    if (from_idx == to_idx) return;

    int small_ind, large_ind;
    if(from_idx < to_idx){
        small_ind = from_idx;
        large_ind = to_idx;
    }
    else{
        small_ind = to_idx;
        large_ind = from_idx;
    }
    
    pthread_mutex_lock(&bank[small_ind].lock);
    pthread_mutex_lock(&bank[large_ind].lock);
    if (bank[from_idx].balance >= amount) {
        bank[from_idx].balance -= amount;
        bank[to_idx].balance += amount;
    }
    pthread_mutex_unlock(&bank[large_ind].lock);
    pthread_mutex_unlock(&bank[small_ind].lock);
}

// Thread Worker Function
void* worker(void* arg) {
    for (int i = 0; i < TRANSACTIONS_PER_THREAD; i++) {
        int from = rand() % NUM_ACCOUNTS;
        int to = rand() % NUM_ACCOUNTS;
        double amount = (rand() % 10) + 1.0;

        transfer(from, to, amount);
    }
    return NULL;
}

int main() {
    srand(time(NULL));
    
    pthread_t threads[NUM_THREADS];

    // Initialize Bank
    double total_start = 0;
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        bank[i].id = i;
        bank[i].balance = INITIAL_BALANCE;
        pthread_mutex_init(&bank[i].lock,NULL);
        total_start += INITIAL_BALANCE;
    }

    printf("--- HFT SIMULATOR STARTING ---\n");
    printf("Initial Total Bank Balance: $%.2f\n", total_start);

    // Launch Threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, worker, NULL);
    }

    // Join Threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Final Audit
    double total_end = 0;
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        total_end += bank[i].balance;
    }

    printf("--- SIMULATION COMPLETE ---\n");
    printf("Final Total Bank Balance:   $%.2f\n", total_end);
    printf("Difference (The Leak):      $%.2f\n", total_end - total_start);

    if (total_end != total_start) {
        printf("RESULT: [FAILED] The bank has lost/created money due to race conditions.\n");
    } else {
        printf("RESULT: [PASSED] The bank is mathematically sound.\n");
    }
    
    for(int i = 0; i < NUM_ACCOUNTS; i++)
        pthread_mutex_destroy(&bank[i].lock);

    return 0;
}
