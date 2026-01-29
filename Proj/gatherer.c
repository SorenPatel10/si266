#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "gatherer.h"

int poll_cpu(cpu_data_t *cpu) {
    static unsigned long long prev_total = 0;
    static unsigned long long prev_idle  = 0;

    FILE *fp = fopen("/proc/stat", "r");
    if (!fp) return 1;

    char line[256];
    unsigned long long user, nice, system, idle;
    unsigned long long iowait, irq, softirq, steal;

    if (!fgets(line, sizeof(line), fp)) {
        fclose(fp);
        return 1;
    }

    fclose(fp);

    if (sscanf(line, "cpu %llu %llu %llu %llu %llu %llu %llu %llu",&user, &nice, &system, &idle,&iowait, &irq, &softirq, &steal) < 8) {
        return 1;
    }

    unsigned long long idle_time  = idle + iowait;
    unsigned long long total_time = user + nice + system + idle + iowait + irq + softirq + steal;

    if (prev_total == 0) {
        prev_total = total_time;
        prev_idle  = idle_time;
        cpu->cpu_usage = 0.0f;
        return 0;
    }

    unsigned long long delta_total = total_time - prev_total;
    unsigned long long delta_idle  = idle_time  - prev_idle;

    prev_total = total_time;
    prev_idle  = idle_time;

    if (delta_total == 0) {
        cpu->cpu_usage = 0.0f;
        return 0;
    }

    cpu->cpu_usage = 1.0f - ((float)delta_idle / (float)delta_total);

    return 0;
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

    if (mem_total == 0) return 1;

    mem->mem_used = (float)(mem_total - mem_avail) / mem_total;
    mem->mem_avail = (float)mem_avail / mem_total;
    mem->mem_free = (float)mem_free / mem_total;
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
    fscanf(fp, "%f %f %f %d/%d",&load->load_1,&load->load_5,&load->load_15,&run,&total);

    cpu->proc_running = run;
    cpu->proc_total   = total;

    fclose(fp);
    return 0;
}

int main(void) {
    cpu_data_t  cpu;
    mem_data_t  mem;
    load_data_t load;

    poll_cpu(&cpu);

    usleep(1000000);

    if (poll_cpu(&cpu) || poll_mem(&mem) || poll_load(&load, &cpu)) {
        return 1;
    }

    printf(
        "CPU_USAGE:%.2f,"
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
        cpu.cpu_usage,
        mem.mem_used,
        mem.mem_avail,
        mem.mem_free,
        mem.mem_cached,
        mem.swap_used,
        mem.swap_free,
        load.load_1,
        load.load_5,
        load.load_15,
        cpu.proc_running,
        cpu.proc_total
    );

    return 0;
}

