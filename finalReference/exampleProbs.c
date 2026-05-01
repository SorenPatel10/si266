// ==============================
// 1️⃣ Basic Signal Handler + sigaction
// ==============================
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

volatile sig_atomic_t flag = 0;

void handler(int sig) {
    flag = 1;
}

int main() {
    struct sigaction sa = {0};
    sa.sa_handler = handler;
    sa.sa_flags = SA_RESTART;

    sigaction(SIGINT, &sa, NULL);

    while (!flag) {
        pause();
    }

    printf("Signal received!\n");
    return 0;
}


// ==============================
// 2️⃣ pthreads + mutex (fix race condition)
// ==============================
#include <pthread.h>
#include <stdio.h>

int counter = 0;
pthread_mutex_t lock;

void* worker(void* arg) {
    for (int i = 0; i < 100000; i++) {
        pthread_mutex_lock(&lock);
        counter++;
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}


// ==============================
// 3️⃣ Deadlock Example (BAD)
// ==============================
// Two threads locking in opposite order → deadlock risk


// ==============================
// 4️⃣ Deadlock Fix (Ordering Locks)
// ==============================
void safe_transfer(int a, int b) {
    if (a < b) {
        pthread_mutex_lock(&bank[a]);
        pthread_mutex_lock(&bank[b]);
    } else {
        pthread_mutex_lock(&bank[b]);
        pthread_mutex_lock(&bank[a]);
    }

    // critical section

    pthread_mutex_unlock(&bank[a]);
    pthread_mutex_unlock(&bank[b]);
}


// ==============================
// 5️⃣ Semaphore Example
// ==============================
#include <semaphore.h>

sem_t sem;

void* task(void* arg) {
    sem_wait(&sem);
    printf("In critical section\n");
    sem_post(&sem);
    return NULL;
}


// ==============================
// 6️⃣ Condition Variable (Producer/Consumer)
// ==============================
#include <pthread.h>

pthread_mutex_t mtx;
pthread_cond_t cond;
int ready = 0;

void* consumer(void* arg) {
    pthread_mutex_lock(&mtx);
    while (!ready) {
        pthread_cond_wait(&cond, &mtx);
    }
    pthread_mutex_unlock(&mtx);
}


// ==============================
// 7️⃣ Basic Client Socket
// ==============================
#include <netdb.h>
#include <sys/socket.h>

int connect_to_server(char* host, char* port) {
    struct addrinfo hints = {0}, *res;
    hints.ai_socktype = SOCK_STREAM;

    getaddrinfo(host, port, &hints, &res);

    int sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    connect(sock, res->ai_addr, res->ai_addrlen);

    freeaddrinfo(res);
    return sock;
}


// ==============================
// 8️⃣ Basic Multithreaded Server
// ==============================
#include <arpa/inet.h>

void* handle(void* arg) {
    int client = *(int*)arg;
    free(arg);

    char buf[1024];
    int n = read(client, buf, sizeof(buf));
    write(client, buf, n);

    close(client);
    return NULL;
}