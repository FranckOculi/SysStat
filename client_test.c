#include "serialize.h"

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
        fprintf(stderr, "(Client) The server is down.\n");
        exit(EXIT_FAILURE);
    }

    const char message[] = "Ping!";

    /* This is the loop to ping server every 2 seconds and receive system stats. */
    
    while(1) {
        if ((send(data_socket, message, strlen(message), 0)) == -1) {
            perror("(Client) send");
            break;
        }

        uint8_t buffer[SYSTEM_STATS_BUFFER_SIZE];
        
        int received_bytes = recv(data_socket, buffer, SYSTEM_STATS_BUFFER_SIZE, 0);
        if (received_bytes == -1) {
            perror("(Server) recv");
            exit(EXIT_FAILURE);
        } else if (received_bytes != SYSTEM_STATS_BUFFER_SIZE) {
            fprintf(stderr, "Received incomplete data\n");
            exit(EXIT_FAILURE);
        }

        struct system_stats stats;
        deserialize_system_stats(&stats, buffer);

        printf("CPU user: %lu\n", stats.cpu.user);
        printf("Memory total: %lu\n", stats.mem.mem_total);
        printf("Uptime: %dh %dm\n", stats.uptime_hours, stats.uptime_minutes);

        sleep(2);
    }

    close(data_socket);

    return EXIT_SUCCESS;
}