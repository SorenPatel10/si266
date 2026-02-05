#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdbool.h>

bool contains(int *arr, int n, int target) {
    int halfway = n / 2;
    int status;
    int pid = fork();

    //child search
    if (pid == 0) {
        for (int i = 0; i < halfway; i++) {
            if (arr[i] == target)
                exit(1);
        }
        exit(0);
    }
    //parent search if the child didnt find it
    else {
        waitpid(pid, &status, 0);

        if (WIFEXITED(status) && WEXITSTATUS(status) == 1)
            return true;

        for (int i = halfway; i < n; i++) {
            if (arr[i] == target)
                return true;
        }
    }

    return false;
}


int main(){

    int arr[] = {1,3,5,7,9,11,13,15,17,19};
    int n = sizeof(arr) / sizeof(arr[0]);

    if(contains(arr,n,15)){
        printf("Found!\n");
    }
    else{
        printf("Not Found.\n");
    }

    return 0;
}