#include "system.h"
#include "common.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

void clean_up(FILE *fptr) {
    if (fptr != NULL) fclose(fptr);
};

static int cpu_info(struct cpu_stats *cpu) {
    if (cpu == NULL) {
        print_log(stderr, "cpu_info : NULL pointer provided\n");
        return -1;
    }

    FILE *fptr;    
    fptr = fopen("/proc/stat", "r");

    if (fptr == NULL) {
        print_log(stderr, "");
        perror("cpu_info : stat not open");
        return -1;
    }

    char label[8];
    if (fscanf(fptr, "%s %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu", 
        label, &cpu->user, &cpu->nice, &cpu->system, &cpu->idle, &cpu->iowait, 
        &cpu->irq, &cpu->softirq, &cpu->steal, &cpu->guest, &cpu->guest_nice) != 11) {
        
        print_log(stderr, "cpu_info : unexpected format\n");
        clean_up(fptr);
        return -1;
    };

    cpu->total = cpu->user + cpu->nice + cpu->system + cpu->idle + cpu->iowait +
    cpu->irq + cpu->softirq + cpu->steal;
    cpu->idle_time = cpu->idle + cpu->iowait;

    clean_up(fptr);
    return 0;
}

static int mem_info(struct mem_stats *mem) {
    if (mem == NULL) {
        print_log(stderr, "mem_info : NULL pointer provided\n");
        return -1;
    }

    FILE *fptr;
    fptr = fopen("/proc/meminfo", "r");

    if (fptr == NULL) {
        print_log(stderr, "");
        perror("mem_info : meminfo not open");
        return -1;
    }

    char line[128];
    char label[32];
    unsigned long long value = 0;

    mem->mem_total = 0;
    mem->mem_available = 0;

    while (fgets(line, sizeof(line), fptr)) {
        if(sscanf(line, "%31[^:]: %llu", label, &value) == 2) {
            if (strcmp(label, "MemTotal") == 0) mem->mem_total = value;
            else if(strcmp(label, "MemAvailable") == 0) mem->mem_available = value;
        }
    }

    if (mem->mem_total == 0 || mem->mem_available == 0) {
        print_log(stderr, "mem_info : failed to read memTotal or memAvailable\n");
        clean_up(fptr);
        return -1;
    } 

    clean_up(fptr);
    return 0;
}

static int uptime_info(int *hours, int *minutes) {
    if (hours == NULL || minutes == NULL) {
        print_log(stderr, "uptime_info : NULL pointer provided\n");
        return -1;
    }

    FILE *fptr;
    fptr = fopen("/proc/uptime", "r");
    if (fptr == NULL) {
        print_log(stderr, "");
        perror("uptime_info : uptime not open");
        return -1;
    }

    double uptime, idle;

    if (fscanf(fptr, "%lf %lf", &uptime, &idle) != 2) {
        print_log(stderr, "uptime_info : failed to read data\n");
        clean_up(fptr);
        return -1;
    }

    *hours = (int)(uptime / 3600);
    *minutes = (int)(uptime / 60) % 60;

    clean_up(fptr);
    return 0;
}

int system_infos(struct system_stats *system_stats) {
    if (system_stats == NULL) {
        print_log(stderr, "system_infos : NULL pointer provided\n");
        return -1;
    }

    int output = 0;
    if ((output = cpu_info(&system_stats->cpu)) != 0) return output;
    if ((output = mem_info(&system_stats->mem)) != 0) return output;
    if ((output = uptime_info(&system_stats->uptime_hours, &system_stats->uptime_minutes)) != 0) return output;

    return output;
}
