#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

volatile sig_atomic_t signal_recieved = 0;

void sigusr1_handler(int signum){
    signal_recieved=1;
}

int main(){
    struct sigaction sa;
    sigset_t mask, oldmask, tempmask;
    int count;
    int rounds = 10000;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigusr1_handler;
    sa.sa_flags = SA_RESTART;

    if(sigaction(SIGUSR1, &sa, NULL)==-1){
        perror("sigatcion fail");
        exit(1);
    }

    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &mask, &oldmask);
    sigemptyset(&tempmask);

    pid_t pid = fork();
    if(pid == 0){
        for(count = 1; count <= rounds; count++){
            while(!signal_recieved){
                sigsuspend(&tempmask);
            }

            signal_recieved = 0;
            if(count % 1000 == 0)
                printf("[Child] Recieved Ping %d... sending Pong!\n", count);

            kill(getppid(), SIGUSR1);
        }
        exit(0);
    }
    else{
        for(count = 1; count <= rounds; count++){
            kill(pid, SIGUSR1);

            while(!signal_recieved)
                sigsuspend(&tempmask);

            signal_recieved = 0;
            if(count % 1000 == 0)  
                printf("[Parent] Recieved Pong %d... sending Ping!\n", count);
        }
        printf("[Parent] Finished 10000 rounds...\n");
        wait(NULL);
    }
    
    return(0);
}