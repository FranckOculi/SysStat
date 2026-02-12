#ifndef COMMON_H
#define COMMON_H

#include "system.h"

#include "stdio.h"

int calcul_cpu_active(const struct system_stats *current, const struct system_stats *prev);
int calcul_mem_active(const struct system_stats *current);

void print_cpu(int cpu);
void print_mem(int mem);
void print_uptime(int hours, int minutes);

void print_log(FILE *stream, const char *fmt, ...);

#endif