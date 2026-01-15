#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "student.h"

Student* create_student(const char* first, const char* last, int id, double gpa) {
    
    Student* temp = malloc(sizeof(Student));
    temp->first_name = malloc(strlen(first) + 1);
    temp->last_name  = malloc(strlen(last) + 1);
    
    strncpy(temp->first_name, first, strlen(first)+1);
    strncpy(temp->last_name, last, strlen(last)+1);
    temp->id = id;
    temp->gpa = gpa;

    return temp;
}

void free_student(Student* s) {
    free(s->first_name);
    free(s->last_name);
    free(s);
}

void print_student(Student* s) {
    printf("[%d] %s, %s (GPA: %.2f)\n",s->id, s->last_name, s->first_name, s->gpa);
}

int main() {
    Student* s = create_student("Grace", "Hopper", 42, 5.00);
    print_student(s);
    free_student(s);
    return 0;
}



