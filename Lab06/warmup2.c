#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){

    int fd[2];
    pipe(fd);
    pid_t pid = fork();

    if(pid==0){
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO); 
        close(fd[1]);

        execlp("ls","ls","-l", NULL);
        exit(1);
    }
    else{
        close(fd[1]);

        char buf;
        int count = 0;
        while(read(fd[0], &buf, 1) >0)
            count++;

        close(fd[0]);
        printf("Captured %d bytes from ls.\n", count);
        wait(NULL);
    }
    
}