#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "student.h"

/* Student* create_student(const char* first_name, const char* last_name, int id, double gpa) {
    
    Student* temp = malloc(sizeof(Student));
    temp->first_name = malloc(strlen(first) + 1);
    temp->last_name  = malloc(strlen(last) + 1);
    
    strncpy(temp->first_name, first, strlen(first)+1);
    strncpy(temp->last_name, last, strlen(last)+1);
    temp->id = id;
    temp->gpa = gpa;

    return temp;
}
*/

Student* create_student_from_string(char* full_name, int id, double gpa) {
    Student* s = malloc(sizeof(Student));

    char* start = full_name;
    char* ptr = full_name;

    while (*ptr != ' ' && *ptr != '\0') {
        ptr++;
    }

    int first_name_size = ptr - start;

    s->first_name = malloc(first_name_size + 1);
    memcpy(s->first_name, start, first_name_size);
    s->first_name[first_name_size] = '\0';
    ptr++;

    char* last_name_start = ptr;

    while (*ptr != '\0') {
        ptr++;
    }

    int last_name_size = ptr - last_name_start;

    s->last_name = malloc(last_name_size + 1);
    memcpy(s->last_name, last_name_start, last_name_size);
    s->last_name[last_name_size] = '\0';

    s->id = id;
    s->gpa = gpa;

    return s;
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
    Student* s = create_student_from_string("Grace Hopper", 42, 5.00);
    print_student(s);
    free_student(s);
    return 0;
}
