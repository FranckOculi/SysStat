#include "systat.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>

void cleanUp(FILE *fptr) {
    if (fptr != NULL) fclose(fptr);
};

static int cpuStat(struct CpuStat *stat) {
    if (stat == NULL) {
        fprintf(stderr, "cpuStat : NULL pointer provided\n");
        return -1;
    }

    FILE *fptr;    
    fptr = fopen("/proc/stat", "r");

    if (fptr == NULL) {
        perror("cpuStat : stat not open");
        return -1;
    }

    char label[8];
    if (fscanf(fptr, "%s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu", 
        label, &stat->user, &stat->nice, &stat->system, &stat->idle, &stat->iowait, 
        &stat->irq, &stat->softirq, &stat->steal, &stat->guest, &stat->guest_nice) != 11) {
        
        fprintf(stderr, "cpuStat : unexpected format\n");
        cleanUp(fptr);
        return -1;
    };

    stat->total = stat->user + stat->nice + stat->system + stat->idle + stat->iowait +
    stat->irq + stat->softirq + stat->steal;
    stat->idleTime = stat->idle + stat->iowait;

    cleanUp(fptr);
    return 0;
}

static int memStat(struct MemStat *stat) {
    if (stat == NULL) {
        fprintf(stderr, "memStat : NULL pointer provided\n");
        return -1;
    }

    FILE *fptr;
    fptr = fopen("/proc/meminfo", "r");

    if (fptr == NULL) {
        perror("uptimeStat : meminfo not open");
        return -1;
    }

    char line[128];
    char label[32];
    unsigned long value = 0;

    stat->memTotal = 0;
    stat->memActive = 0;

    while (fgets(line, sizeof(line), fptr)) {
        if(sscanf(line, "%31[^:]: %lu", label, &value) == 2) {
            if (strcmp(label, "MemTotal") == 0) stat->memTotal = value;
            else if(strcmp(label, "Active") == 0) stat->memActive = value;
        }
    }

    if (stat->memTotal == 0 || stat->memActive == 0) {
        fprintf(stderr, "memStat : failed to read memTotal or memActive\n");
        cleanUp(fptr);
        return -1;
    } 

    cleanUp(fptr);
    return 0;
}

static int uptimeStat(int *hours, int *minutes) {
    if (hours == NULL || minutes == NULL) {
        fprintf(stderr, "uptimeStat : NULL pointer provided\n");
        return -1;
    }

    FILE *fptr;
    fptr = fopen("/proc/uptime", "r");
    if (fptr == NULL) {
        perror("uptimeStat : uptime not open");
        return -1;
    }

    double uptime, idle;

    if (fscanf(fptr, "%lf %lf", &uptime, &idle) != 2) {
        fprintf(stderr, "uptimeStat : failed to read data\n");
        cleanUp(fptr);
        return -1;
    }

    *hours = (int)(uptime / 3600);
    *minutes = (int)(uptime / 60) % 60;

    cleanUp(fptr);
    return 0;
}

int getSystemStats(struct SystemStats *stats) {
    if (stats == NULL) {
        fprintf(stderr, "getSystemStats : NULL pointer provided\n");
        return -1;
    }

    int output = 0;
    if ((output = cpuStat(&stats->cpu)) != 0) return output;
    if ((output = memStat(&stats->mem)) != 0) return output;
    if ((output = uptimeStat(&stats->uptimeHours, &stats->uptimeMinutes)) != 0) return output;

    return output;
}
