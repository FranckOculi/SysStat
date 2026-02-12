#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_FAMILY AF_UNIX
#define BUFFER_SIZE 1024
#define SOCKET_NAME "/tmp/sysstat.sock"

int main(void) {
    /* Create local socket.  */
    int data_socket = socket(SOCKET_FAMILY, SOCK_STREAM, 0);
    if (data_socket == -1) {
        print_log(stderr, "");
        perror("(Client) socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un socket_addr;
    int socket_addr_size = sizeof(socket_addr);

    /*
    * For portability clear the whole structure, since some implementations have 
    * additional (nonstandard) fields in the structure.
    */
    memset(&socket_addr, 0, socket_addr_size);

    /* Connect socket to socket address.  */
    socket_addr.sun_family = AF_UNIX;
    strncpy(socket_addr.sun_path, SOCKET_NAME, sizeof(socket_addr.sun_path) - 1);

    int connect_return_code = connect(data_socket, (const struct sockaddr*) &socket_addr, socket_addr_size);
    if (connect_return_code == -1) {
        print_log(stderr, "(Client) The server is down.\n");
        exit(EXIT_FAILURE);
    }

    const char message[] = "Ping!";

    /* This is the loop to ping server every 2 seconds and receive system stats. */
    struct system_stats prev_buffer = {0};
    struct system_stats current_buffer = {0};
    unsigned int first_loop = 1;
    
    while(1) {
        if ((send(data_socket, message, strlen(message), 0)) == -1) {
            print_log(stderr, "");
            perror("(Client) send");
            break;
        }

        int received_bytes = recv(data_socket, &current_buffer, sizeof(current_buffer), 0);
        if (received_bytes == -1) {
            print_log(stderr, "");
            perror("(Server) recv");
            exit(EXIT_FAILURE);
        } else if (received_bytes != sizeof(current_buffer)) {
            print_log(stderr, "Received incomplete data\n");
            exit(EXIT_FAILURE);
        }

        if (first_loop == 1) {
            first_loop = 0;
        } else {
            print_cpu(calcul_cpu_active(&current_buffer, &prev_buffer));
        }

        print_mem(calcul_mem_active(&current_buffer));
        print_uptime(current_buffer.uptime_hours, current_buffer.uptime_minutes);

        prev_buffer = current_buffer;

        sleep(2);
    }

    close(data_socket);

    return EXIT_SUCCESS;
}