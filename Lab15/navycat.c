#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define BUFFER_SIZE 1024


void *listen_thread(void *arg) {
    int sockfd = *(int *)arg;
    char buffer[BUFFER_SIZE];
    int bytes;

    while (1) {
        bytes = read(sockfd, buffer, BUFFER_SIZE);

        if (bytes <= 0) {
            break;
        }

        write(STDOUT_FILENO, buffer, bytes);
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <hostname> <port>\n", argv[0]);
        exit(1);
    }

    char *hostname = argv[1];
    char *port = argv[2];

    struct addrinfo hints, *res, *p;
    int status;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(hostname, port, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    int sockfd;

    for (p = res; p != NULL; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1) {
            perror("socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            perror("connect");
            close(sockfd);
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "Failed to connect\n");
        freeaddrinfo(res);
        exit(2);
    }

    freeaddrinfo(res);

    printf("Connected to %s:%s\n", hostname, port);

    pthread_t tid;
    if (pthread_create(&tid, NULL, listen_thread, &sockfd) != 0) {
        perror("pthread_create");
        close(sockfd);
        exit(1);
    }

    char buffer[BUFFER_SIZE];
    int bytes;

    while (1) {
        bytes = read(STDIN_FILENO, buffer, BUFFER_SIZE);

        if (bytes <= 0) {
            break;
        }

        write(sockfd, buffer, bytes);
    }

    close(sockfd);
    pthread_cancel(tid);
    pthread_join(tid, NULL);

    return 0;
}