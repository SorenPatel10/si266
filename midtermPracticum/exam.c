#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include "exam.h" // Prototypes defined here

// Tier 1: Easy
int count_char(char *filename, char target) {
    FILE* fp = fopen(filename, "r");
    if(fp == NULL){
        return -1;
    }

    int tracker = 0;
    int next=fgetc(fp);

    
    while(next > 0){
        if(next == target){
            tracker+=1;
        }
        next = fgetc(fp); 
    }
    fclose(fp);
    return tracker;

}
int read_middle_bytes(char *filename) {
  return -1; //CHANGE THIS
}

void spawn_and_check() {
    int child_pid = fork();
    if(child_pid==0){
        int modded = getpid() % 100;
        exit(modded);
    }
    else{
        int status;
        wait(&status);
        if(WIFEXITED(status)){
            int code = WEXITSTATUS(status);
            printf("Child exited with code %d\n", code);
        }
    }
}

void run_custom_exec(char *prog, char *flag) {
    printf("Replacing process...\n");
    char input[1024];
    char *argv[64];

    argv[0] = prog;
    argv[1] = flag;

    execvp(argv[0],argv);
    printf("This should not print\n");
}

// Tier 2: Medium
void spawn_fleet(int n) {
    int x = n;
    while(n > 0){
        if(fork() == 0){
            printf("Ship %d reporting\n", getpid());
            exit(0);
        }
        else{
            wait(NULL);
            n-=1;
        }
    }
    printf("Fleet assembled\n");
    
}
void run_number_doubler(int val) {
  return;
}
void run_file_relay(char *cmd, char *temp_filename) {
  return;
}