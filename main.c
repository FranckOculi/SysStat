#include "systat.h"
#include <stdio.h>
#include <unistd.h>

#define RED   "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define RESET "\033[0m"

void printCPU(int cpu) {
    if (cpu > 80) printf(RED "CPU: %.d%%\n" RESET, cpu);
    else if (cpu > 40) printf(YELLOW "CPU: %d%%\n" RESET, cpu);
    else printf(GREEN "CPU: %.d%%\n" RESET, cpu);
}

void printMEM(int mem) {
    if (mem > 80) printf(RED "Mem: %.d%%\n" RESET, mem);
    else if (mem > 40) printf(YELLOW "Mem: %.d%%\n" RESET, mem);
    else printf(GREEN "Mem: %d%%\n" RESET, mem);
}

void printUptime(int hours, int minutes) {
    if (hours >= 5) printf(RED "Uptime: %02d:%02d\n" RESET, hours, minutes);
    else if (hours > 2) printf(YELLOW "Uptime: %02d:%02d\n" RESET, hours, minutes);
    else printf(GREEN "Uptime: %02d:%02d\n" RESET, hours, minutes);
}

int main(void) {
    printf("Starting system stats monitor...\n");

    struct SystemStats prev = {0};
    struct SystemStats current = {0};

    int output = 0;
    output = getSystemStats(&prev);
    unsigned long deltaTotal, deltaUsage = 0;
    float cpuActive = 0.0;

    while(output != 1) {
        sleep(2);
   
        current = (struct SystemStats) {0};
        if ((output = getSystemStats(&current)) != 0) {
            return 1;
        }

        deltaTotal = current.cpu.total - prev.cpu.total;
        deltaUsage = (current.cpu.total - current.cpu.idleTime) - (prev.cpu.total - prev.cpu.idleTime);
        if (deltaTotal > 0) {
            cpuActive = 100.00 * (float)deltaUsage / (float)deltaTotal;
        } else {
            cpuActive = 0.0;
        }

        printCPU((int)cpuActive);
        
        if (current.mem.memTotal != 0 && current.mem.memAvailable != 0) {
            printMEM(100.0 * (current.mem.memTotal - current.mem.memAvailable) / current.mem.memTotal);
        } else {
            printf("Mem: ---\n");
        }
    
        printUptime(current.uptimeHours, current.uptimeMinutes);

        prev = current;
    };

    return output;
}