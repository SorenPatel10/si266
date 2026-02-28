#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "gatherer.h"
#include <signal.h>
#include <sys/wait.h>

//global variable
volatile sig_atomic_t keep_running = 1;

//changes to 0, stops the loop
void sigint_handler(int sig){
    keep_running = 0;
}

//calculating cpu usage
int calc_cpu(cpu_data_stats *cpu) {
    FILE *fp;
    char line[256];

    //proc/stat fields to be read in 
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
    unsigned long long total1, work1, total2, work2;
    
    //open file and error check
    fp = fopen("/proc/stat", "r");
    if (!fp)
        return 1;
    if (!fgets(line, sizeof(line), fp)) { 
        fclose(fp);
        return 1;
    }
    fclose(fp);

    //get all cpu values from file
    if (sscanf(line, "cpu %llu %llu %llu %llu %llu %llu %llu %llu",
               &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal) < 8)
        return 1;

    work1  = user + nice + system + irq + softirq + steal;
    total1 = work1 + idle + iowait;

    //sleep to calcualte cpu delta
    sleep(1);

    //open again and read 2nd time
    fp = fopen("/proc/stat", "r");
    if (!fp)
        return 1;
    if (!fgets(line, sizeof(line), fp)) {
        fclose(fp);
        return 1;
    }
    fclose(fp);

    if (sscanf(line, "cpu %llu %llu %llu %llu %llu %llu %llu %llu",
               &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal) < 8) {
        return 1;
    }

    work2  = user + nice + system + irq + softirq + steal;
    total2 = work2 + idle + iowait;

    //calculate deltas between cpu readings
    unsigned long long delta_total = total2 - total1;
    unsigned long long delta_work  = work2 - work1;

    //calculate final value and store into struct
    if (delta_total == 0)
        cpu->cpu_usage = 0.0;
    else
        cpu->cpu_usage = ((double)delta_work / (double)delta_total) * 100.0;

    return 0;
}

//calculating memory use
int calc_mem(mem_data_stats *mem) {
    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp) return 1;

    //variables to store fields from proc/meminfo
    char key[64], unit[16];
    unsigned long long value;
    unsigned long long mem_total = 0, mem_free = 0, mem_avail = 0;
    unsigned long long cached = 0, swap_total = 0, swap_free = 0;

    //read all fields
    while (fscanf(fp, "%63s %llu %15s\n", key, &value, unit) == 3) {
        if (strcmp(key, "MemTotal:") == 0)
            mem_total = value;
        else if (strcmp(key, "MemFree:") == 0)
            mem_free = value;
        else if (strcmp(key, "MemAvailable:") == 0)
            mem_avail = value;
        else if (strcmp(key, "Cached:") == 0)
            cached = value;
        else if (strcmp(key, "SwapTotal:") == 0)
            swap_total = value;
        else if (strcmp(key, "SwapFree:") == 0)
            swap_free = value;
    }

    fclose(fp);

    if (mem_total == 0)
        return 1;

    //compute memory fractions
    mem->mem_used = (float)(mem_total - mem_avail) / mem_total;
    mem->mem_avail = (float)mem_avail / mem_total;
    mem->mem_free = (float)mem_free / mem_total;
    mem->mem_cached = (float)cached / mem_total;

    //store values into struct
    if (swap_total == 0) {
        mem->swap_used = 0.0f;
        mem->swap_free = 1.0f;
    }
    else{
        mem->swap_used = (float)(swap_total - swap_free) / swap_total;
        mem->swap_free = (float)swap_free / swap_total;
    }

    return 0;
}

//calculating system load
int calc_load(load_data_stats *load, cpu_data_stats *cpu) {
    FILE *fp = fopen("/proc/loadavg", "r");
    if (!fp)
        return 1;

    //format string to read
    int run = 0, total = 0;
    if (fscanf(fp, "%f %f %f %d/%d",
               &load->load_1, &load->load_5, &load->load_15, &run, &total) != 5) {
        fclose(fp);
        return 1;
    }

    //store values
    cpu->proc_running = run;
    cpu->proc_total = total;

    fclose(fp);
    return 0;
}

void update_loop(){
    cpu_data_stats cpu;
    mem_data_stats mem;
    load_data_stats load;

    //loop depending on close signal
    while(keep_running){
        //call each method, print error if ret anything other than 0
        if (calc_cpu(&cpu) || calc_mem(&mem) || calc_load(&load, &cpu)) {
            fprintf(stderr, "Error reading system\n");
        }
        //print relevant info in correct format
        else{
            printf("CPU_USAGE:%.2f,"
                   "MEM_USED:%.2f,"
                   "MEM_AVAIL:%.2f,"
                   "MEM_FREE:%.2f,"
                   "MEM_CACHED:%.2f,"
                   "SWAP_USED:%.2f,"
                   "SWAP_FREE:%.2f,"
                   "LOAD_1:%.2f,"
                   "LOAD_5:%.2f,"
                   "LOAD_15:%.2f,"
                   "PROC_RUN:%d,"
                   "PROC_TOTAL:%d\n",
                cpu.cpu_usage,mem.mem_used,mem.mem_avail,mem.mem_free,
                mem.mem_cached,mem.swap_used,mem.swap_free,
                load.load_1,load.load_5,load.load_15,
                cpu.proc_running,cpu.proc_total);
        }
        //flush stdout print all
        fflush(stdout);
    }
}

//main function
int main(void) {
    
    //create pipe and error check
    int fd[2];
    if(pipe(fd) == -1){
        perror("pipe has failed");
        exit(1);
    }

    //signal handler setup and error check
    struct sigaction sa;
    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if(sigaction(SIGINT, &sa, NULL)==-1){
        perror("sigaction failure");
        exit(1);
    }

    //fork and erorr check
    pid_t child_pid = fork();
    if (child_pid < 0) {
        perror("fork failed");
        exit(1);
    }

    //child process action, run the visualizer
    if(child_pid == 0){
        //pipe end management with error checking
        if (close(fd[1]) == -1) {
            perror("close failed");
            exit(1);
        }
        if (dup2(fd[0], STDIN_FILENO) == -1) {
            perror("dup2 failed");
            exit(1);
        }
        if (close(fd[0]) == -1) {
            perror("close failed");
            exit(1);
        }
        execl("./visualizer", "visualizer",NULL);
        perror("exec has failed");
        exit(1);
    }
    //parent process action, call update_loop
    else{
        //pipe end management with error checking
        if (close(fd[0]) == -1) {
            perror("close failed");
            exit(1);
        }
        if (dup2(fd[1], STDOUT_FILENO) == -1) {
            perror("dup2 failed");
            exit(1);
        }
        if (close(fd[1]) == -1) {
            perror("close failed");
            exit(1);
        }
        
        update_loop();

        if (close(STDOUT_FILENO) == -1) {
            perror("close failed");
            exit(1);
        }
        if (wait(NULL) == -1) {
            perror("wait failed");
            exit(1);
        }
    }

    return 0;
}