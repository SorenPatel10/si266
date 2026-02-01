#ifndef GATHERER_H
#define GATHERER_H

typedef struct {
    float cpu_usage;
    int proc_running;
    int proc_total;
} cpu_data_stats;

typedef struct {
    float mem_used;
    float mem_avail;
    float mem_free;
    float mem_cached;
    float swap_used;
    float swap_free;
} mem_data_stats;

typedef struct {
    float load_1;
    float load_5;
    float load_15;
} load_data_stats;

int poll_cpu(cpu_data_stats *cpu);
int poll_mem(mem_data_stats *mem);
int poll_load(load_data_stats *load, cpu_data_stats *cpu);

#endif
