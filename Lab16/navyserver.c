#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>


void *handle_client(void *arg){
    pthread_detach(pthread_self());

    int client_sock = *(int*)arg;
    free(arg);

    char buf[1024];
    ssize_t bytes_read;

    while(1){
        bytes_read = read(client_sock, buf, sizeof(buf));
        if(bytes_read < 0){
            perror("read fail");
            break;
        }
        else if(bytes_read == 0){
            break;
        }

        ssize_t total_write = 0;
        while(total_write < bytes_read){
            ssize_t bytes_write = write(client_sock, buf+total_write, bytes_read-total_write);

            if(bytes_write<0){
                perror("write fail");
                close(client_sock);
                return NULL;
            }
            total_write += bytes_write;
        }
    }

    close(client_sock);
    return NULL;

}


int main(int argc, char* argv[]){
    if(argc != 2){
        fprintf(stderr, "format: %s <port>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);
    if(port <= 0){
        fprintf(stderr, "invalid port\n");
        return 1;
    }

    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sock < 0){
        perror("socket fail");
        return 1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if(bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
        perror("binding fail");
        close(server_sock);
        return 1;
    }

    if (listen(server_sock, 10)< 0) {
        perror("listening fail");
        close(server_sock);
        return 1;
    }

    while(1){
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_len);
        if(client_sock < 0){
            perror("accepting fail");
            continue;
        }

        pthread_t worker;
        int* safe_sock = malloc(sizeof(int));
        if(!safe_sock){
            perror("malloc fail");
            close(client_sock);
            continue;
        }

        *safe_sock = client_sock;

        if(pthread_create(&worker, NULL, handle_client, safe_sock)!=0){
            perror("create thread fail");
            close(client_sock);
            free(safe_sock);
        }

    }

    close(server_sock);
    return 0;
}