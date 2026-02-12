#include "common.h"

#include "stdio.h"
#include "time.h"
#include "stdarg.h"

#define RED   "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define RESET "\033[0m"

int calcul_cpu_active(const struct system_stats *current, const struct system_stats *prev) {
    unsigned long long delta_total, delta_usage = 0;

    delta_total = current->cpu.total - prev->cpu.total;
    delta_usage = (current->cpu.total - current->cpu.idle_time) - (prev->cpu.total - prev->cpu.idle_time);
    if (delta_total > 0) {
        return 100.00 * delta_usage / delta_total;
    } 

    return 0.0;
}

int calcul_mem_active(const struct system_stats *current) {
    if (current->mem.mem_total != 0 && current->mem.mem_available != 0) {
        return 100.0 * (current->mem.mem_total - current->mem.mem_available) / current->mem.mem_total;
    }

    return 0;
}

void print_cpu(int cpu) {
    if (cpu > 80) printf(RED "CPU: %.d%%\n" RESET, cpu);
    else if (cpu > 40) printf(YELLOW "CPU: %d%%\n" RESET, cpu);
    else printf(GREEN "CPU: %.d%%\n" RESET, cpu);
}

void print_mem(int mem) {
    if (mem == 0) printf("Mem: ---\n");
    else if (mem > 80) printf(RED "Mem: %.d%%\n" RESET, mem);
    else if (mem > 40) printf(YELLOW "Mem: %.d%%\n" RESET, mem);
    else printf(GREEN "Mem: %d%%\n" RESET, mem);
}

void print_uptime(int hours, int minutes) {
    if (hours >= 5) printf(RED "Uptime: %02d:%02d\n" RESET, hours, minutes);
    else if (hours > 2) printf(YELLOW "Uptime: %02d:%02d\n" RESET, hours, minutes);
    else printf(GREEN "Uptime: %02d:%02d\n" RESET, hours, minutes);
}

void print_log(FILE *stream, const char *fmt, ...) {
    time_t current_time = time(NULL);
    struct tm *tm_info = localtime(&current_time);

    char buffer[32];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm_info);
    fprintf(stream, "[%s] - ", buffer);

    va_list args;
    va_start(args, fmt);
    vfprintf(stream, fmt, args);
    va_end(args);
}