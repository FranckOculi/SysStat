#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>

#define SOCKET_FAMILY AF_UNIX
#define MAX_CONNECTION_REQUEST 1
#define BUFFER_SIZE 1024
#define SOCKET_NAME "/tmp/sysstat.sock"

int connection_socket = -1;
int connected_socket = -1;
int stop = 0;

void handleSignal(int signum) {
    (void)signum;
    stop = 1;

    if (connected_socket != -1) {
        printf("(Server) Caught user request to close the connection.\n");
        /* To force stopping recv. */
        shutdown(connected_socket, SHUT_RDWR);
        close(connected_socket);
        connected_socket = -1;
    }
}


int main(void) {
    signal(SIGINT, handleSignal);

    /* Create local socket.  */

    connection_socket = socket(SOCKET_FAMILY, SOCK_STREAM, 0);
    if (connection_socket == -1) {
        perror("(Server) socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un socket_name;
    int socket_name_size = sizeof(socket_name);

    /*
    * For portability clear the whole structure, since some implementations have 
    * additional (nonstandard) fields in the structure.
    */

    memset(&socket_name, 0, socket_name_size);
    
    /* Bind socket to socket name.  */

    socket_name.sun_family = SOCKET_FAMILY;
    strncpy(socket_name.sun_path, SOCKET_NAME, sizeof(socket_name.sun_path) - 1);

    int bind_return_code = bind(connection_socket, (const struct sockaddr*) &socket_name, socket_name_size);
    if (bind_return_code == -1) {
        perror("(Server) bind");
        exit(EXIT_FAILURE);
    }

    /*
    * Prepare for accepting connections.  The backlog size is set to 2.  
    * So while one request is being processed other request can be waiting.
    */

    if(listen(connection_socket, MAX_CONNECTION_REQUEST) == -1) {
        perror("(Server) listen");
        exit(EXIT_FAILURE);
    };

    puts("Waiting for new connection...");

    /* This is the main loop for handling connections.  */

    for(;;) {
        if (stop) break;
        connected_socket = accept(connection_socket, NULL, NULL);
        if (connected_socket == -1) {
            if (stop) break;
            perror("(Server) accept");
            exit(EXIT_FAILURE);
        }
    
        char buffer[BUFFER_SIZE];

        /* This is the loop to communicate with only one client. */

        int received_bytes;
        while((received_bytes = recv(connected_socket, buffer, BUFFER_SIZE - 1, 0)) > 0) {

            /* Ensure buffer is 0-terminated.  */

            buffer[received_bytes] = '\0';
            printf("(Server) message received : %s\n", buffer);

            /* Send a response. */

            const char message[] = "Hy client, I'm the server";
            if ((send(connected_socket, message, strlen(message), 0)) == -1) {
                perror("(Server) send");
                break;
            }
        }

        if (received_bytes == -1) {
            if (stop) break;
            perror("(Server) recv");
            // break;
        } else if (received_bytes == 0) {
            printf("(Server) client disconnected.\n");
            close(connected_socket);
            connected_socket = -1;
        }
    }

    close(connected_socket);
    close(connection_socket);

    /* Unlink the socket.  */

    unlink(SOCKET_NAME);

    return EXIT_SUCCESS;
}