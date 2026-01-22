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

        if (strncmp(currLine, "ID:", 3) != 0) {
            continue;
        }

        sscanf(currLine, "ID: %d | %f %f %f", &id, &f1, &f2, &f3);
        float avg = (f1+f2+f3) / 3.0;
        printf("Sensor %d: Avg Temp = %.2f\n", id, avg);
    }
    
    return 0;
}