#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "gatherer.h"
#include <signal.h>
#include <sys/wait.h>
#include <pthread.h>

//global variable
volatile sig_atomic_t keep_running = 1;

//shared struct to hold gathered metrics
typedef struct{
    cpu_data_stats cpu;
    mem_data_stats mem;
    load_data_stats load;

    //flags to signal go and ready for each
    int go_cpu, go_mem, go_load;
    int ready_cpu, ready_mem, ready_load;

    //mutex and condition variable
    pthread_mutex_t lock;
    pthread_cond_t cond;
} system_data;

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

//cpu working thread
void* cpu_thread(void* arg){
    system_data *data = (system_data*)arg;

    while(keep_running){
        //use mutex to control critical section
        pthread_mutex_lock(&data->lock);
        while(!data->go_cpu && keep_running){
            //wait for signal
            pthread_cond_wait(&data->cond,&data->lock);
        }

        if(!keep_running){
            pthread_mutex_unlock(&data->lock);
            break;
        }

        data->go_cpu = 0;
        pthread_mutex_unlock(&data->lock);

        //gather CPU info
        cpu_data_stats temp;
        if(calc_cpu(&temp)==0){
            pthread_mutex_lock(&data->lock);
            data->cpu = temp;
            //change signals accordingly
            data->ready_cpu = 1;
            pthread_cond_broadcast(&data->cond);
            pthread_mutex_unlock(&data->lock);
        }
    }
    return NULL;
}

//memory working thread
void* mem_thread(void* arg){
    system_data *data = (system_data*)arg;

    while(keep_running){
        //use mutex to control crit section
        pthread_mutex_lock(&data->lock);
        while(!data->go_mem && keep_running){
            //wait for signal
            pthread_cond_wait(&data->cond,&data->lock);
        }

        if(!keep_running){
            pthread_mutex_unlock(&data->lock);
            break;
        }

        data->go_mem = 0;
        pthread_mutex_unlock(&data->lock);

        mem_data_stats temp;
        if(calc_mem(&temp)==0){
            pthread_mutex_lock(&data->lock);
            data->mem = temp;
            //change signals accordingly
            data->ready_mem = 1;
            pthread_cond_broadcast(&data->cond);
            pthread_mutex_unlock(&data->lock);
        }
    }
    return NULL;
}

//load working thread
void* load_thread(void* arg){
    system_data *data = (system_data*)arg;

    while(keep_running){
        //mutex to control critical section
        pthread_mutex_lock(&data->lock);
        while(!data->go_load && keep_running){
            //wait for signal
            pthread_cond_wait(&data->cond,&data->lock);
        }

        if(!keep_running){
            pthread_mutex_unlock(&data->lock);
            break;
        }

        data->go_load = 0;
        pthread_mutex_unlock(&data->lock);

        load_data_stats temp_load;
        cpu_data_stats temp_cpu;
        if(calc_load(&temp_load,&temp_cpu)==0){
            pthread_mutex_lock(&data->lock);
            data->load = temp_load;
            data->cpu.proc_running = temp_cpu.proc_running;
            data->cpu.proc_total = temp_cpu.proc_total;
            //change signals accordingly
            data->ready_load = 1;
            pthread_cond_broadcast(&data->cond);
            pthread_mutex_unlock(&data->lock);
        }
    }
    return NULL;
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
    //preventing broken pipe error message on ctrl c exit
    signal(SIGPIPE, SIG_IGN);

    //set out space for system data struct and error check
    system_data *data = malloc(sizeof(system_data));
    if(!data){
        perror("malloc fail");
        exit(1);
    }

    //initialize flags
    //start with all go flags at 1, all ready flags at 0
    data->go_cpu = 1;
    data->go_mem = 1;
    data->go_load = 1;
    data->ready_cpu = 0;
    data->ready_mem = 0;
    data->ready_load = 0;

    //intialize mutex and conditional var
    pthread_mutex_init(&data->lock, NULL);
    pthread_cond_init(&data->cond, NULL);

    //make threads and call thread functions
    //one thread for each proc file to read
    pthread_t tcpu, tmem, tload;
    pthread_create(&tcpu, NULL, cpu_thread, data);
    pthread_create(&tmem, NULL, mem_thread, data);
    pthread_create(&tload, NULL, load_thread, data);

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
        
        //the rest of the update loop functionality
        while(keep_running){
            
            //lock control to prevent race condition
            pthread_mutex_lock(&data->lock);

            while(!(data->ready_cpu && data->ready_mem && data->ready_load) && keep_running){
                //wait for signal to continue
                pthread_cond_wait(&data->cond,&data->lock);
            }
            if(!keep_running){
                pthread_mutex_unlock(&data->lock);
                break;
            }
            //take single unified snapshot 
            cpu_data_stats cpu = data->cpu;
            mem_data_stats mem = data->mem;
            load_data_stats load = data->load;

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

            fflush(stdout);

            //change flags accordingly, ready = 0 and go = 1 for next loop
            data->ready_cpu = 0;
            data->ready_mem = 0;
            data->ready_load = 0;
            data->go_cpu = 1;
            data->go_mem = 1;
            data->go_load = 1;

            //wake up workers and unlock lock
            pthread_cond_broadcast(&data->cond);
            pthread_mutex_unlock(&data->lock);

            sleep(1);
        }

        //wake up sleeping workers to shutdown gracefuly
        pthread_mutex_lock(&data->lock);
        pthread_cond_broadcast(&data->cond);
        pthread_mutex_unlock(&data->lock);

        //join all threads
        pthread_join(tcpu,NULL);
        pthread_join(tmem,NULL);
        pthread_join(tload,NULL);

        //destroy mutex and cond var
        pthread_mutex_destroy(&data->lock);
        pthread_cond_destroy(&data->cond);

        //free the malloced struct
        free(data);

        //collect child
        if (wait(NULL) == -1) {
            perror("wait failed");
            exit(1);
        }
    }
    printf("\nGraceful shutdown initiated. Cleaning up...\n");

    return 0;
}