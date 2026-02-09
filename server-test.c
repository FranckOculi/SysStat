#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define SOCKET_FAMILY AF_INET
#define LISTENING_PORT  5094
#define MAX_CONNECTION_REQUEST 1
#define BUFFER_SIZE 1024


int main(void) {
    int socketFD = socket(SOCKET_FAMILY, SOCK_STREAM, 0);
    if (socketFD == -1) {
        printf("(Server) Fail to initialize socket\n");
        perror("(Server) socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in socketAddress;
    socketAddress.sin_family = SOCKET_FAMILY;
    socketAddress.sin_port = LISTENING_PORT;
    socketAddress.sin_addr.s_addr = INADDR_ANY; // We accept connection from any address

    int socketAddressLength = sizeof(socketAddress);
    int bindReturnCode = bind(socketFD, (struct sockaddr*) &socketAddress, socketAddressLength);
    if (bindReturnCode == -1) {
        printf("(Server) Fail to bind socket\n");
        perror("(Server) bind");
        exit(EXIT_FAILURE);
    }

    if(listen(socketFD, MAX_CONNECTION_REQUEST) == -1) {
        printf("(Server) Fail to listening on the socket\n");
        perror("(Server) listen");
        exit(EXIT_FAILURE);
    };

    puts("Waiting for new connection...");

    int connectedSocketFD = accept(socketFD, (struct sockaddr*) &socketAddress, (socklen_t*) &socketAddressLength);
    if (connectedSocketFD == -1) {
        printf("(Server) Fail to connect on socket\n");
        perror("(Server) accept");
        exit(EXIT_FAILURE);
    }

    char clientIP[INET_ADDRSTRLEN];
    inet_ntop(SOCKET_FAMILY, &socketAddress.sin_addr, clientIP, INET_ADDRSTRLEN);

    printf("(Server) Connected with FD %d from %s:%d\n", connectedSocketFD, clientIP, LISTENING_PORT);

    // receive message
    char buffer[BUFFER_SIZE] = {0};

    while(1) {
        int receivedBytes = recv(connectedSocketFD, buffer, BUFFER_SIZE, 0);
        if (receivedBytes == -1) {
            printf("(Server) Fail to receiv message from %d\n", connectedSocketFD);
            perror("(Server) recv");
            break;
        } else if (receivedBytes == 0) {
            printf("(Server) client disconnected.\n");
            break;
        }

        printf("(Server) message received : %s\n", buffer);

        // send response
        const char message[] = "Hy client, I'm the server";
        if ((send(connectedSocketFD, message, strlen(message), 0)) == -1) {
            printf("(Server) Fail to send message response to %d\n", connectedSocketFD);
            perror("(Server) send");
            break;
        }
    }

    close(connectedSocketFD);
    close(socketFD);

    return EXIT_SUCCESS;
}