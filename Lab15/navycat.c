#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define BUF_SIZE 1024

void *listen_thread(void *arg) {
    int socket_fd = *(int*)arg;
    char buf[BUF_SIZE];
    int bytesRead;

    while(1) {
        bytesRead = read(socket_fd, buf, BUF_SIZE);

        if (bytesRead == 0) {
            exit(0);
        }
        else if (bytesRead < 0) {
            perror("read");
            exit(1);
        }

        write(STDOUT_FILENO, buf, bytesRead);
    }
    return NULL;
}

int main(int argc, char *argv[]) {

    if (argc != 3) {
        fprintf(stderr, "ERROR: require arguments\n");
        exit(1);
    }

    char *hostname = argv[1];
    char *port = argv[2];

    struct addrinfo hints, *results, *p;
    int status;

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM; 
    status = getaddrinfo(hostname, port, &hints, &results);

    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(1);
    }

    int socket_fd;

    for(p = results; p != NULL; p = p->ai_next){

        socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if(socket_fd < 0){
            perror("socket");
            continue;
        }

        if(connect(socket_fd, p->ai_addr, p->ai_addrlen) < 0) {
            perror("connect");
            close(socket_fd);
            continue;
        }

        break;
    }

    if (p == NULL) {
        freeaddrinfo(results);
        exit(1);
    }

    freeaddrinfo(results);

    pthread_t listener;
    if (pthread_create(&listener, NULL, listen_thread, &socket_fd) != 0) {
        perror("pthread_create");
        close(socket_fd);
        exit(1);
    }

    char buf[BUF_SIZE];
    int bytesRead;

    while(1) {
        bytesRead = read(STDIN_FILENO, buf, BUF_SIZE);

        if(bytesRead == 0)
            break;
        
        else if (bytesRead < 0){
            perror("read");
            break;
        }

        if(write(socket_fd, buf, bytesRead) < 0) {
            perror("write");
            break;
        }
    }

    close(socket_fd);
    return 0;
}