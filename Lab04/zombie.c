#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(){

    if(fork() == 0){
        exit(0);
    }
    else{
        printf("Zombie created. Run 'ps -l' now.\n");
        sleep(10);
    }

    return 0;
}