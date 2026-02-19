#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){
    
    int fd1[2];
    int fd2[2];
    pipe(fd1);
    pipe(fd2);

    if(fork()==0){
        dup2(fd1[1],STDOUT_FILENO);

        close(fd1[0]);
        close(fd1[1]);
        close(fd2[0]);
        close(fd2[1]);
        
        execlp("ls","ls","-l",NULL);
        exit(1);
    }
    
    if(fork()==0){
        dup2(fd1[0],STDIN_FILENO);
        dup2(fd2[1],STDOUT_FILENO);

        close(fd1[0]);
        close(fd1[1]);
        close(fd2[0]);
        close(fd2[1]);
        
        execlp("grep","grep","r",NULL);
        exit(1);
    }

    if(fork()==0){
        dup2(fd2[0],STDIN_FILENO);

        close(fd1[0]);
        close(fd1[1]);
        close(fd2[0]);
        close(fd2[1]);
        
        execlp("wc","wc","-l",NULL);
        exit(1);
    }

    close(fd1[0]);
    close(fd1[1]);
    close(fd2[0]);
    close(fd2[1]);
    wait(NULL);
    wait(NULL);
    wait(NULL);

    return 0;

}