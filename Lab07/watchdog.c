#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

volatile sig_atomic_t heartbeat = 0;
volatile sig_atomic_t timeout = 0;

void sigalrm_handler(int signum){
    timeout = 1;
}

void sigusr1_handler(int signum){
    heartbeat = 1;
}

int main(){
    struct sigaction sa_sigusr1;
    struct sigaction sa_sigalrm;
    sigset_t mask, tempmask;
    
    memset(&sa_sigusr1, 0, sizeof(sa_sigusr1));
    sa_sigusr1.sa_handler = sigusr1_handler;
    sa_sigusr1.sa_flags = SA_RESTART;
    sigaction(SIGUSR1, &sa_sigusr1, NULL);

    memset(&sa_sigalrm, 0, sizeof(sa_sigalrm));
    sa_sigalrm.sa_handler = sigalrm_handler;
    sa_sigalrm.sa_flags = SA_RESTART;
    sigaction(SIGALRM, &sa_sigalrm, NULL);   

    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGALRM);
    sigprocmask(SIG_BLOCK, &mask, NULL);
    sigemptyset(&tempmask);

    pid_t child_pid = fork();
    if(child_pid == 0){
        while(1){
            sleep(1);
            printf("[Worker] Doing work...\n");
            kill(getppid(), SIGUSR1);
        }
        exit(0);
    }
    else{
        
        while(1){
            while(!timeout && !heartbeat){
                sigsuspend(&tempmask);
            }

            if(heartbeat){
                heartbeat = 0;
                alarm(3);
            }
            if(timeout){
                timeout = 0;
                printf("[WATCHDOG] Child unresponsive! Terminating...\n");
                kill(child_pid, SIGKILL);
                wait(NULL);

                child_pid = fork();
                if(child_pid == 0){
                    while(1){
                        sleep(1);
                        printf("[Worker] Doing work...\n");
                        kill(getppid(), SIGUSR1);
                    }
                    exit(0);
                }
                alarm(3);
            }
        }
    }
    return 0;
}