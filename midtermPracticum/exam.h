#ifndef EXAM_H
#define EXAM_H

// Tier 1: Easy
int count_char(char *filename, char target);
int read_middle_bytes(char *filename);
void spawn_and_check();
void run_custom_exec(char *prog, char *flag);

// Tier 2: Medium
void spawn_fleet(int n);
void run_number_doubler(int val);
void run_file_relay(char *cmd, char *temp_filename);

#endif
