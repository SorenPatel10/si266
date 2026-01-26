#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gatherer.h"

int poll_cpu(cpu_data_t *cpu) {
    FILE *fp = fopen("/proc/stat", "r");
    if (!fp) return 1;

    char line[256];
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
    int got_cpu = 0;

    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "cpu ", 4) == 0) {
            sscanf(line, "cpu %llu %llu %llu %llu %llu %llu %llu %llu",
                   &user, &nice, &system, &idle,
                   &iowait, &irq, &softirq, &steal);

            unsigned long long idle_time = idle + iowait;
            unsigned long long total_time =
                user + nice + system + idle + iowait + irq + softirq + steal;

            cpu->cpu_usage = 1.0f - ((float)idle_time / (float)total_time);
            got_cpu = 1;
        } else if (sscanf(line, "procs_running %d", &cpu->proc_running) == 1) {
            continue;
        } else if (sscanf(line, "processes %d", &cpu->proc_total) == 1) {
            continue;
        }
    }

    fclose(fp);
    return got_cpu ? 0 : 1;
}

int poll_mem(mem_data_t *mem) {
    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp) return 1;

    char key[64];
    unsigned long long value;
    char unit[16];

    unsigned long long mem_total = 0, mem_free = 0, mem_avail = 0;
    unsigned long long cached = 0, swap_total = 0, swap_free = 0;

    while (fscanf(fp, "%63s %llu %15s\n", key, &value, unit) == 3) {
        if (strcmp(key, "MemTotal:") == 0) mem_total = value;
        else if (strcmp(key, "MemFree:") == 0) mem_free = value;
        else if (strcmp(key, "MemAvailable:") == 0) mem_avail = value;
        else if (strcmp(key, "Cached:") == 0) cached = value;
        else if (strcmp(key, "SwapTotal:") == 0) swap_total = value;
        else if (strcmp(key, "SwapFree:") == 0) swap_free = value;
    }

    fclose(fp);

    if (mem_total == 0) return 1;

    mem->mem_used   = (float)(mem_total - mem_avail) / mem_total;
    mem->mem_avail  = (float)mem_avail / mem_total;
    mem->mem_free   = (float)mem_free / mem_total;
    mem->mem_cached = (float)cached / mem_total;

    if (swap_total == 0) {
        mem->swap_used = 0.0f;
        mem->swap_free = 1.0f;
    } else {
        mem->swap_used = (float)(swap_total - swap_free) / swap_total;
        mem->swap_free = (float)swap_free / swap_total;
    }

    return 0;
}

int poll_load(load_data_t *load, cpu_data_t *cpu) {
    FILE *fp = fopen("/proc/loadavg", "r");
    if (!fp) return 1;

    int run, total;
    fscanf(fp, "%f %f %f %d/%d",
           &load->load_1,
           &load->load_5,
           &load->load_15,
           &run,
           &total);

    cpu->proc_running = run;
    cpu->proc_total   = total;

    fclose(fp);
    return 0;
}

int main(void) {
    cpu_data_t *cpu = malloc(sizeof(cpu_data_t));
    mem_data_t *mem = malloc(sizeof(mem_data_t));
    load_data_t *load = malloc(sizeof(load_data_t));

    if (!cpu || !mem || !load) {
        free(cpu);
        free(mem);
        free(load);
        return 2;
    }

    if (poll_cpu(cpu) ||
        poll_mem(mem) ||
        poll_load(load, cpu)) {

        free(cpu);
        free(mem);
        free(load);
        return 1;
    }

    printf("CPU_USAGE:%.2f,""MEM_USED:%.2f,""MEM_AVAIL:%.2f,""MEM_FREE:%.2f,""MEM_CACHED:%.2f,"
        "SWAP_USED:%.2f,""SWAP_FREE:%.2f,""LOAD_1:%.2f,""LOAD_5:%.2f,""LOAD_15:%.2f,"
        "PROC_RUN:%d,""PROC_TOTAL:%d\n",
        cpu->cpu_usage,mem->mem_used,mem->mem_avail,mem->mem_free,mem->mem_cached,mem->swap_used,
        mem->swap_free,load->load_1,load->load_5,load->load_15,cpu->proc_running,cpu->proc_total);

    free(cpu);
    free(mem);
    free(load);

    return 0;
}
