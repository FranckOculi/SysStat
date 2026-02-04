#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#define CONVERT_KB_TO_GB 1048576

void cleanUp(FILE *fptr) {
    if (fptr != NULL) {
        fclose(fptr);
    }
};

struct CpuStat {
    unsigned long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
    unsigned long total, idleTime;
};

int cpuStat(struct CpuStat *stat) {
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

int memStat(void) {
    FILE *fptr;
    fptr = fopen("/proc/meminfo", "r");

    if (fptr == NULL) {
        perror("meminfo not open\n");
        return 1;
    }

    char label[15];
    unsigned long value = 0, memTotal = 0, memActive = 0;
    char unit[3];

    for(int i = 0; i < 7; i++) {
        if (fscanf(fptr, "%s %lu %s", label, &value, unit) == 3) {
            if (strcmp(label, "MemTotal:") == 0) {
                memTotal = value;
            } else if(strcmp(label, "Active:") == 0) {
                memActive = value;
            }
        }
    }

    if (memTotal != 0 && memActive != 0) {
        printf("Mem: %.2fG/%.fG\n", (float)memActive / CONVERT_KB_TO_GB, ceilf((float)memTotal / CONVERT_KB_TO_GB));
    } else {
        printf("Mem ---\n");
    }

    cleanUp(fptr);
    return 0;
}

int uptimeStat(void) {
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

    int hours, minutes;
    hours = (int)(uptime / 3600);
    minutes = (int)(uptime / 60) % 60;

    printf("uptime: %02d:%02d\n", hours, minutes);

    cleanUp(fptr);
    return 0;
}

int main(void) {
    printf("Start stat\n");

    struct CpuStat prev = {0};
    struct CpuStat next = {0};

    int output = 0;
    output = cpuStat(&prev);
    unsigned long deltaTotal, deltaUsage = 0;
    float cpuActive = 0.0;

    while(output != 1) {
        sleep(2);
   
        if ((output = cpuStat(&next)) != 0) {
            return 1;
        }

        deltaTotal = next.total - prev.total;
        deltaUsage = (next.total - next.idleTime) - (prev.total - prev.idleTime);
        if (deltaTotal > 0) {
            cpuActive = 100.00 * (float)deltaUsage / (float)deltaTotal;
        } else {
            cpuActive = 0.0;
        }

        printf("CPU: %.1f %%\n", cpuActive);
        prev = next;
        deltaTotal = 0;
        deltaUsage = 0;
        
        if ((output = memStat()) != 0) {
            return 1;
        }

        if ((output = uptimeStat()) != 0) {
            return 1;
        }
    };

    return output;
}