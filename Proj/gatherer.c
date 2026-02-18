#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "gatherer.h"

int calc_cpu(cpu_data_stats *cpu) {
    FILE *fp;
    char line[256];
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
    unsigned long long total1, work1, total2, work2;
    
    fp = fopen("/proc/stat", "r");
    if (!fp)
        return 1;
    if (!fgets(line, sizeof(line), fp)) { 
        fclose(fp);
        return 1;
    }
    fclose(fp);

    if (sscanf(line, "cpu %llu %llu %llu %llu %llu %llu %llu %llu", &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal) < 8)
        return 1;

    work1  = user + nice + system + irq + softirq + steal;
    total1 = work1 + idle + iowait;

    sleep(1);

    fp = fopen("/proc/stat", "r");
    if (!fp)
        return 1;
    if (!fgets(line, sizeof(line), fp)) {
        fclose(fp);
        return 1;
    }
    fclose(fp);

    if (sscanf(line, "cpu %llu %llu %llu %llu %llu %llu %llu %llu",&user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal) < 8) {
        return 1;
    }

    work2  = user + nice + system + irq + softirq + steal;
    total2 = work2 + idle + iowait;

    unsigned long long delta_total = total2 - total1;
    unsigned long long delta_work  = work2 - work1;

    if (delta_total == 0)
        cpu->cpu_usage = 0.0;
    else
        cpu->cpu_usage = ((double)delta_work / (double)delta_total) * 100.0;

    return 0;
}

int calc_mem(mem_data_stats *mem) {
    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp) return 1;

    char key[64], unit[16];
    unsigned long long value;
    unsigned long long mem_total = 0, mem_free = 0, mem_avail = 0;
    unsigned long long cached = 0, swap_total = 0, swap_free = 0;

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

    mem->mem_used = (float)(mem_total - mem_avail) / mem_total;
    mem->mem_avail = (float)mem_avail / mem_total;
    mem->mem_free = (float)mem_free / mem_total;
    mem->mem_cached = (float)cached / mem_total;

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

int calc_load(load_data_stats *load, cpu_data_stats *cpu) {
    FILE *fp = fopen("/proc/loadavg", "r");
    if (!fp)
        return 1;

    int run = 0, total = 0;
    if (fscanf(fp, "%f %f %f %d/%d",&load->load_1, &load->load_5, &load->load_15, &run, &total) != 5) {
        fclose(fp);
        return 1;
    }

    cpu->proc_running = run;
    cpu->proc_total = total;

    fclose(fp);
    return 0;
}

void update_loop(){
    cpu_data_stats cpu;
    mem_data_stats mem;
    load_data_stats load;

    while(1){
        if (calc_cpu(&cpu) || calc_mem(&mem) || calc_load(&load, &cpu)) {
            fprintf(stderr, "Error reading system\n");
        }
        else{
            printf("CPU_USAGE:%.2f,""MEM_USED:%.2f,""MEM_AVAIL:%.2f,""MEM_FREE:%.2f,""MEM_CACHED:%.2f,""SWAP_USED:%.2f,""SWAP_FREE:%.2f,""LOAD_1:%.2f,""LOAD_5:%.2f,""LOAD_15:%.2f,""PROC_RUN:%d,""PROC_TOTAL:%d\n",
                cpu.cpu_usage,mem.mem_used,mem.mem_avail,mem.mem_free,mem.mem_cached,mem.swap_used,mem.swap_free,load.load_1,load.load_5,load.load_15,cpu.proc_running,cpu.proc_total);
        }
        fflush(stdout);
    }
}

int main(void) {
    
    pid_t child_pid = fork();

    if(child_pid == 0){
        execl("./visualizer", "visualizer",NULL);
        perror("exec has failed");
        exit(1);
    }
    else{
        update_loop();
    }

    return 0;
}
