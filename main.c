#include "network.h"
#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#define LOGFILE "/tmp/daemon_test.log"
#define PIDFILE "/tmp/daemon_test.pid"

void close_log_file() {
    unlink(PIDFILE);
}

int check_existing_pid(const char* pidfile) {
    FILE *f = fopen(pidfile, "r");
    if (f == NULL) return 0;

    int pid = 0;
    fscanf(f, "%d", &pid);
    fclose(f);

    if (pid <= 0) return 0;

    /* A process already exist. */
    if (kill(pid, 0) == 0) {
        return 1;
    } else if (errno == ESRCH) {
        return 0;
    }

    return 0;
}

void demonize() {
    if (check_existing_pid(PIDFILE)) {
        print_log(stderr, "Daemon is already running!\n");
        exit(EXIT_FAILURE);
    }

    pid_t pid = 0;
    
    /* Fork the calling process.  */
    pid = fork();

    if (pid < 0) {
        print_log(stderr, "Daemon (fork)\n");
        perror("Daemon (fork)\n");
        exit(EXIT_FAILURE);
    } 
    
    /* Terminate the parent process. */
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    /* Create a new session and become its leader. */
    if (setsid() < 0) {
        print_log(stderr, "Daemon (setsid)\n");
        perror("Daemon (setsid)\n");
        exit(EXIT_FAILURE);
    }

    /* Ignore child termination and terminal hangup. */
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    // signal(SIGTERM, SIG_IGN);

    /* Fork again so the daemon is no longer a session leader. */
    pid = fork();

    if (pid < 0) {
        print_log(stderr, "Error : (setsid - seconde)\n");
        perror("Daemon (setsid - seconde) \n");
        exit(EXIT_FAILURE);
    };

    /* Terminate the parent process (again). */
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    /* Clear the file mode creation mask (permission). */
    umask(0);
    chdir("/");

    /* close standard file descriptors. */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    /* Open or create the log file. */
    int log_fd = open(LOGFILE, O_RDWR | O_CREAT | O_APPEND, 0600);
    if (log_fd < 0) {
        exit(EXIT_FAILURE);
    };

    /* Redirect stdout and stderr to the log file.  */
    dup2(log_fd, STDOUT_FILENO);
    dup2(log_fd, STDERR_FILENO);
    close(log_fd);

    /* Open or create the PID file to store the daemon's PID. */
    int pid_fd = open(PIDFILE, O_RDWR | O_CREAT | O_TRUNC, 0600);
    if (pid_fd < 0) {
        print_log(stderr, "Daemon (open pid file)\n");
        perror("Daemon (open pid file)");
        exit(EXIT_FAILURE);
    }
    
    if (pid_fd >= 0) {
        char str[32];
        snprintf(str, sizeof(str), "%d\n", getpid());
        write(pid_fd, str, strlen(str));
        close(pid_fd);
    }
}

int main(void) {
    demonize();
    print_log(stdout, "Starting system stats monitor...\n");

    return run(close_log_file);
}