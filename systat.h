#ifndef SYSTAT_H
#define SYSTAT_H

#define CONVERT_KB_TO_GB 1048576

struct CpuStat {
    unsigned long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
    unsigned long total, idleTime;
};

struct MemStat {
    unsigned long memTotal, memAvailable;
};

struct SystemStats {
    struct CpuStat cpu;
    struct MemStat mem;
    int uptimeHours;
    int uptimeMinutes;
};

int getSystemStats(struct SystemStats *stats);

#endif