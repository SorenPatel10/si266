#include <stdio.h>
#include <math.h>
#include <string.h>

int main(){

    FILE *ptr = fopen("sensors.dat", "r");
    if(ptr == NULL){
        perror("Error! File does not exist.");
        return 1;
    }

    char currLine[100];

    while(fgets(currLine, sizeof(currLine), ptr)){
        int id;
        float f1, f2, f3;

        if (strncmp(line, "ID:", 3) != 0) {
            continue;
        }

        
    }
    
    return 0;
}