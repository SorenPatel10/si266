#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){
    
    char input[1024];
    char *argv[64]; // Array to hold arguments
    int last_status = 0;

    while(1){
        printf("mysh> ");
        fflush(stdout);

        if(fgets(input,sizeof(input),stdin)==NULL)
            break;
        
        //replacing the newline with null byt
        input[strcspn(input, "\n")] = '\0';

        int i = 0;
        // Get the first token
        argv[i] = strtok(input, " ");
        // Keep getting tokens until NULL
        while(argv[i] != NULL) {
            i++;
            argv[i] = strtok(NULL, " ");
        }

        //preprocessor check
        #ifdef DEBUG
            for (int i = 0; argv[i] != NULL; i++) {
                printf("Token %d: %s\n", i, argv[i]);
            }
        #endif
        
        //commands to handle in the parent logic before forking
        if(argv[0] == NULL) continue;
        if(strcmp(argv[0], "exit") == 0) exit(0);
        if (strcmp(argv[0], "status") == 0) {
            printf("Last command exited with: %d\n", last_status);
            continue;
        }
        
        if(fork() == 0){
            execvp(argv[0],argv);
            printf("Command not found\n");
            _exit(1);
        }
        else{
            //track on status stuff
            int status;
            wait(&status);
            if(WIFEXITED(status))
                last_status = WEXITSTATUS(status);
        }
    }

    return 0;
}
