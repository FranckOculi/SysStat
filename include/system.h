#ifndef SYSTEM_H
#define SYSTEM_H

#define CONVERT_KB_TO_GB 1048576

struct cpu_stats {
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
    unsigned long long total, idle_time;
};

struct mem_stats {
    unsigned long long mem_total, mem_available;
};

struct system_stats {
    struct cpu_stats cpu;
    struct mem_stats mem;
    int uptime_hours;
    int uptime_minutes;
};

int system_infos(struct system_stats *system_stats);

#endif