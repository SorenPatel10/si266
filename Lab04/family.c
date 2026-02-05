#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(void) {
    pid_t child_pid, grandchild_pid;

    child_pid = fork();

    //child runs this
    if (child_pid == 0) {

        grandchild_pid = fork();

        //grandchild runs this
        if (grandchild_pid == 0) {
            printf("[Grandchild] PID: %d, PPID: %d\n",
                   getpid(), getppid());
            exit(0);
        }

        wait(NULL);

        printf("[Child] PID: %d, PPID: %d\n",
               getpid(), getppid());
        exit(0);
    }

    wait(NULL);

    printf("[Parent] PID: %d, PPID: %d\n",
           getpid(), getppid());

    return 0;
}
