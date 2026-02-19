#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char* argv[]){

    if(argc!=2){
        fprintf(stderr, "Error: must provide an argument.\n");
        return 1;
    }

    srand(atoi(argv[1]));

    int fd[2];
    pipe(fd);

    pid_t pid = fork();

    if(pid > 0){
        close(fd[0]);
        for(int i = 0; i < 50; i++){
            int num = rand();
            write(fd[1], &num, sizeof(int));
        }

        close(fd[1]);
        wait(NULL);
    }
    else{
        close(fd[1]);
        int sum = 0;
        int num;

        while(read(fd[0], &num, sizeof(int))>0){
            sum += num;
        }
        close(fd[0]);

        printf("Sum: %d\n", sum);
    }

    return 0;
}