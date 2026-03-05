#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

volatile sig_atomic_t alarm_on = 0;

void sigalrm_handler(int signum){
    alarm_on = 1;
}

int main(){
    
    int sec;
    char response;

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigalrm_handler;
    sa.sa_flags = SA_RESTART;

    if(sigaction(SIGALRM, &sa, NULL)==-1){
        perror("sigaction fail");
        exit(1);
    }
    printf("Enter seconds until alarm: ");
    scanf("%d", &sec);

    alarm(sec);

    while(1){
        pause();
        if(alarm_on){
            printf("BEEP BEEP BEEP! Snooze? (y/n): ");
            scanf(" %c", &response);

            if(response == 'y'){
                alarm_on=0;
                alarm(5);
            }
            else{
                printf("Waking up...\n");
                break;
            }
        }
    }

    return 0;
}