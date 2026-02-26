1️⃣ Swap Two Integers Using Pointers
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

2️⃣ malloc + Struct Allocation
#include <stdlib.h>

struct Point {
    int x;
    int y;
};

struct Point* create_point(int x, int y) {
    struct Point *p = malloc(sizeof(struct Point));
    if (p == NULL) return NULL;

    p->x = x;
    p->y = y;

    return p;
}

3️⃣ Prefix Check with strncmp
#include <string.h>

int starts_with(const char *str, const char *prefix) {
    return strncmp(str, prefix, strlen(prefix)) == 0;
}

4️⃣ File Reading with fgets
#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *fp = fopen("data.txt", "r");
    if (fp == NULL) {
        perror("fopen");
        exit(1);
    }

    char line[256];

    while (fgets(line, sizeof(line), fp) != NULL) {
        printf("%s", line);
    }

    fclose(fp);
    return 0;
}

5️⃣ Basic fork Example
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t child_pid = fork();

    if (child_pid < 0) {
        perror("fork");
        exit(1);
    }

    if (child_pid == 0) {
        printf("Child PID: %d\n", getpid());
    } else {
        wait(NULL);
        printf("Parent PID: %d\n", getpid());
    }

    return 0;
}

6️⃣ wait() and Exit Status
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t child_pid = fork();

    if (child_pid < 0) {
        perror("fork");
        exit(1);
    }

    if (child_pid == 0) {
        exit(7);
    } else {
        int status;
        wait(&status);

        if (WIFEXITED(status)) {
            printf("exit code was %d\n", WEXITSTATUS(status));
        }
    }

    return 0;
}

7️⃣ exec() Example — Run ls -l
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        execlp("ls", "ls", "-l", NULL);
        perror("exec failed");
        exit(1);
    } else {
        wait(NULL);
    }

    return 0;
}

8️⃣ pipe() + dup2() Skeleton (Two-Process Pipeline: ls | wc -l)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int fd[2];
    if (pipe(fd) < 0) { perror("pipe"); exit(1); }

    pid_t pid1 = fork();
    if (pid1 < 0) { perror("fork"); exit(1); }

    if (pid1 == 0) {
        // First child: ls
        close(fd[0]);                // close read end
        dup2(fd[1], STDOUT_FILENO);  // stdout -> pipe
        close(fd[1]);
        execlp("ls", "ls", NULL);
        perror("exec ls");
        exit(1);
    }

    pid_t pid2 = fork();
    if (pid2 < 0) { perror("fork"); exit(1); }

    if (pid2 == 0) {
        // Second child: wc -l
        close(fd[1]);                // close write end
        dup2(fd[0], STDIN_FILENO);   // stdin <- pipe
        close(fd[0]);
        execlp("wc", "wc", "-l", NULL);
        perror("exec wc");
        exit(1);
    }

    // Parent closes both ends
    close(fd[0]);
    close(fd[1]);

    wait(NULL);
    wait(NULL);

    return 0;
}