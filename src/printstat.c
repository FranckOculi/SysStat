#include "system.h"

#include <stdio.h>
#include <unistd.h>

#define RED   "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define RESET "\033[0m"

void print_cpu(int cpu) {
    if (cpu > 80) printf(RED "CPU: %.d%%\n" RESET, cpu);
    else if (cpu > 40) printf(YELLOW "CPU: %d%%\n" RESET, cpu);
    else printf(GREEN "CPU: %.d%%\n" RESET, cpu);
}

void print_mem(int mem) {
    if (mem > 80) printf(RED "Mem: %.d%%\n" RESET, mem);
    else if (mem > 40) printf(YELLOW "Mem: %.d%%\n" RESET, mem);
    else printf(GREEN "Mem: %d%%\n" RESET, mem);
}

void print_uptime(int hours, int minutes) {
    if (hours >= 5) printf(RED "Uptime: %02d:%02d\n" RESET, hours, minutes);
    else if (hours > 2) printf(YELLOW "Uptime: %02d:%02d\n" RESET, hours, minutes);
    else printf(GREEN "Uptime: %02d:%02d\n" RESET, hours, minutes);
}

/* 
* Used only to test system_infos().
* This program does not start the server and only prints system informations
* to the terminal for verification purposes.
*/
int main(void) {
    printf("############# System stats #############\n");

    struct system_stats prev = {0};
    struct system_stats current = {0};

    int output = 0;
    output = system_infos(&prev);
    unsigned long long delta_total, delta_usage = 0;
    float cpu_active = 0.0;

    current = (struct system_stats) {0};
    if ((output = system_infos(&current)) != 0) {
        return 1;
    }

    delta_total = current.cpu.total - prev.cpu.total;
    delta_usage = (current.cpu.total - current.cpu.idle_time) - (prev.cpu.total - prev.cpu.idle_time);
    if (delta_total > 0) {
        cpu_active = 100.00 * (float)delta_usage / (float)delta_total;
    } else {
        cpu_active = 0.0;
    }

    print_cpu((int)cpu_active);
    
    if (current.mem.mem_total != 0 && current.mem.mem_available != 0) {
        print_mem(100.0 * (current.mem.mem_total - current.mem.mem_available) / current.mem.mem_total);
    } else {
        printf("Mem: ---\n");
    }

    print_uptime(current.uptime_hours, current.uptime_minutes);

    prev = current;

    return output;
}