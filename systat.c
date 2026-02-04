#include "systat.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>

void cleanUp(FILE *fptr) {
    if (fptr != NULL) {
        fclose(fptr);
    }
};

static int cpuStat(struct CpuStat *stat) {
    FILE *fptr;    
    fptr = fopen("/proc/stat", "r");

    if (fptr == NULL) {
        perror("cpu stat not open\n");
        return 1;
    }

    char label[6];
    if (fscanf(fptr, "%s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu", 
        label, &stat->user, &stat->nice, &stat->system, &stat->idle, &stat->iowait, 
        &stat->irq, &stat->softirq, &stat->steal, &stat->guest, &stat->guest_nice) != 11) {

        cleanUp(fptr);
        return 1;
    };

    stat->total = stat->user + stat->nice + stat->system + stat->idle + stat->iowait +
    stat->irq + stat->softirq + stat->steal;
    stat->idleTime = stat->idle + stat->iowait;

    cleanUp(fptr);

    return 0;
}

static int memStat(struct MemStat *stat) {
    FILE *fptr;
    fptr = fopen("/proc/meminfo", "r");

    if (fptr == NULL) {
        perror("meminfo not open\n");
        return 1;
    }

    char label[15];
    unsigned long value = 0;
    char unit[3];
    
    for(int i = 0; i < 7; i++) {
        if (fscanf(fptr, "%s %lu %s", label, &value, unit) == 3) {
            if (strcmp(label, "MemTotal:") == 0) {
                stat->memTotal = value;
            } else if(strcmp(label, "Active:") == 0) {
                stat->memActive = value;
            }
        }
    }

    cleanUp(fptr);
    return 0;
}

static int uptimeStat(int *hours, int *minutes) {
    FILE *fptr;
    fptr = fopen("/proc/uptime", "r");
    if (fptr == NULL) {
        perror("uptime not open\n");
        return 1;
    }

    double uptime, idle;

    if (fscanf(fptr, "%lf %lf", &uptime, &idle) != 2) {
        cleanUp(fptr);
        return 1;
    }

    *hours = (int)(uptime / 3600);
    *minutes = (int)(uptime / 60) % 60;


    cleanUp(fptr);
    return 0;
}

int getSystemStats(struct SystemStats *stats) {
    if (cpuStat(&stats->cpu)) return 1;
    if (memStat(&stats->mem)) return 1;
    if (uptimeStat(&stats->uptimeHours, &stats->uptimeMinutes)) return 1;

    return 0;
}
