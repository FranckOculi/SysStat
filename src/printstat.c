#include "system.h"
#include "common.h"

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>

#define INTERVAL 2

void print_verbose(int verbose, const char *format,...) {
    if (verbose) {
        va_list args;
        va_start(args, format);
        printf(format, va_arg(args, int));
        va_end(args);
    }
}

/* 
* Used only to test system_infos().
* This program does not start the server and only prints system informations
* to the terminal for verification purposes.
*/
int main(int argc, char *argv[]) {
    int verbose = 0;
    int opt;

    while((opt = getopt(argc, argv, "v")) != -1) {
        if (opt == 'v') {
            verbose = 1;
        } 
    }

    struct system_stats prev = {0};
    struct system_stats current = {0};

    int output = 0;

    print_verbose(verbose, "############# System stats #############\n");

    if ((output = system_infos(&prev)) != 0) {
        return output;
    }

    print_verbose(verbose, "First metrics : ok\n");
    print_verbose(verbose, "Wait for %d seconds\n", INTERVAL);

    sleep(INTERVAL);

    if ((output = system_infos(&current)) != 0) {
        return output;
    }

    print_verbose(verbose, "Second metrics : ok\n");

    print_cpu(calcul_cpu_active(&current, &prev));
    print_mem(calcul_mem_active(&current));
    print_uptime(current.uptime_hours, current.uptime_minutes);

    prev = current;

    print_verbose(verbose, "############# End #############\n");

    return output;
}