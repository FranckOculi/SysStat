#include "systat.h"
#include <stdio.h>
#include <unistd.h>
#include <math.h>

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

        printf("CPU: %.1f %%\n", cpuActive);
        
        if (current.mem.memTotal != 0 && current.mem.memActive != 0) {
            printf("Mem: %.2fG/%.fG\n", (float)current.mem.memActive / CONVERT_KB_TO_GB, ceilf((float)current.mem.memTotal / CONVERT_KB_TO_GB));
        } else {
            printf("Mem: ---\n");
        }
                
        printf("Uptime: %02d:%02d\n", current.uptimeHours, current.uptimeMinutes);

        prev = current;
    };

    return output;
}